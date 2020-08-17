#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "SessionManager.h"
#include "RouterCaster.h"
#include <boost/bind.hpp>

namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_context& io_context = Singleton<boost::asio::io_context>::GetInstance();

IResponseHandler::IResponseHandler()
	: msg_seq(0)
	, expire_time(0)
	, on_exception(nullptr)
{
}

Session::Session()
	: Network::Tcp::Session()
	, type(TYPE::INVALID)
{
}

Session::~Session()
{
}

void Session::OnCreate()
{
}

void Session::OnAccept()
{
}

bool Session::Init()
{
	if (false == Network::Tcp::Session::Init())
	{
		return false;
	}
	type = TYPE::INVALID;
	msg_seq = 0;

	async_pool_index = 0;
	for(int i=0; i<ASYNC_POOL_SIZE; i++)
	{
		std::shared_ptr<AsyncSession> async = std::make_shared<AsyncSession>();
		async->session_manager = session_manager;
		async_pool[i] = async;
	}
	return true;
}

void Session::OnConnect()
{
	static_cast<SessionManager*>(session_manager)->on_connect(router_address);
}

void Session::OnClose(int reason)
{
	if (TYPE::MASTER == type)
	{
		Singleton<RouterCaster>::GetInstance().UnregisterAddress(router_address);
		static_cast<SessionManager*>(session_manager)->on_close(router_address);
	}
}

void Session::OnDestroy()
{
}

void Session::Close(int reason)
{
	if (nullptr == socket)
	{
		return;
	}

	OnClose(reason);
	socket = nullptr;

	OnDestroy();
	session_manager->Remove(shared_from_this());
}

std::shared_ptr<Tcp::Packet> Session::SyncSend(const std::shared_ptr<Tcp::Packet>& packet, int timeout)
{
	std::shared_ptr<SyncSession> session = sync_pool.Create();
	if (nullptr == session->socket)
	{
		if (false == session->Connect(remote_endpoint))
		{
			return nullptr;
		}
	}
	
	if (0 > session->SyncSend(packet))
	{
		return nullptr;
	}
	return session->SyncRead(timeout);
}

void Session::AsyncSend(const std::shared_ptr<Tcp::Packet>& packet)
{
	std::shared_ptr<AsyncSession> session = async_pool[async_pool_index++ % ASYNC_POOL_SIZE];
	if (nullptr == session->socket)
	{
		if (false == session->Connect(remote_endpoint))
		{
			throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError);
		}
		session->AsyncRead();
	}

	session->AsyncSend(packet);
}

void Session::AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, std::shared_ptr<IResponseHandler>& responseHandler, std::function<void(const Exception&)>& onException, int seconds)
{
	std::shared_ptr<AsyncSession> session = async_pool[async_pool_index++ % ASYNC_POOL_SIZE];
	if (nullptr == session->socket)
	{
		if (false == session->Connect(remote_endpoint))
		{
			throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError);
		}
		session->AsyncRead();
	}

	session->AsyncSend(packet, responseHandler, onException, seconds);
}

SyncSession::SyncSession() 
{
	connector.connect_handler = std::bind(&SyncSession::OnConnect, this, std::placeholders::_1);
}

SyncSession::~SyncSession() 
{
}

bool SyncSession::Init()
{
	return true;
}

bool SyncSession::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	return connector.SyncConnect(endpoint.address().to_v4().to_string(), endpoint.port(), 5);
}

void SyncSession::OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
	Session::Init();
	this->socket = socket;
	
	MsgRouter_RegisterAddress_Ntf ntf;
	ntf.router_address = Singleton<SessionManager>::GetInstance().local_address;

	std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
	packet->Write(ntf);
	SyncSend(packet);
}

void SyncSession::Close(int reason)
{
	auto self = shared_from_this();
	Dispatch([this, self](){
		expire_timer.Cancel();
		socket = nullptr;
	});
}

std::shared_ptr<Tcp::Packet> SyncSession::SyncRead(int timeout)
{
	auto self = shared_from_this();
	std::promise<std::shared_ptr<Tcp::Packet>> promise;
	
	// promise 써줘야 함. 다른 스레드에서 호출 됨.
	Dispatch([this, self, &promise, timeout](){
		if (nullptr == socket)
		{
			promise.set_value(nullptr);
			return;
		}

		std::shared_ptr<boost::asio::ip::tcp::socket> socket = this->socket;

		bool expire = false;
		expire_timer.AutoReset(false);
		expire_timer.SetTimer(timeout * 1000, [socket, &expire]() {
			expire = true;
			socket->cancel();
		});

		std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
		do {
			if (0 == packet->Available())
			{
				throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "buffer overflow(read size:", packet->length, ")");
			}

			try {
				int readBytes = socket->read_some(boost::asio::buffer(packet->WritePtr(), packet->Available()));
				if (0 == readBytes)
				{
					throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "buffer overflow(read size:", packet->length, ")");
				}
				packet->write_index += readBytes;
			}
			catch (const boost::system::system_error& e)
			{
				if (true == expire)
				{
					promise.set_value(nullptr);
					return;
				}
				expire_timer.Cancel();
				Close(ErrorCode::Success);
				throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "buffer overflow(read size:", packet->length, ")");
			}
			if (Tcp::Packet::HEADER_SIZE > (int)packet->Size())
			{
				continue;
			}

			packet->ReadHeader();

			if (Tcp::Packet::HEADER_SIZE > packet->length)
			{
				throw GAMNET_EXCEPTION(ErrorCode::BufferUnderflowError, "buffer underflow(read size:", packet->length, ")");
			}

			if (packet->length >= (uint16_t)packet->Capacity())
			{
				throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "buffer overflow(read size:", packet->length, ")");
			}
		} while (packet->length > (uint16_t)packet->Size());

		expire_timer.Cancel();
		promise.set_value(packet);
	});
	return promise.get_future().get();
}

AsyncSession::AsyncSession()
{
	connector.connect_handler = std::bind(&AsyncSession::OnConnect, this, std::placeholders::_1);
}

bool AsyncSession::Init()
{
	return true;
}

bool AsyncSession::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	return connector.SyncConnect(endpoint.address().to_v4().to_string(), endpoint.port(), 5);
}

void AsyncSession::OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
	Session::Init();

	this->socket = socket;

	MsgRouter_RegisterAddress_Ntf ntf;
	ntf.router_address = Singleton<SessionManager>::GetInstance().local_address;

	std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
	packet->Write(ntf);
	AsyncSend(packet);
}

void AsyncSession::AsyncSend(const std::shared_ptr<Tcp::Packet>& packet) 
{
	Network::Session::AsyncSend(packet);
}

void AsyncSession::AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, std::shared_ptr<IResponseHandler>& responseHandler, std::function<void(const Exception&)>& onException, int seconds)
{
	responseHandler->msg_seq = packet->msg_seq;
	responseHandler->expire_time = time(nullptr) + seconds;
	responseHandler->on_exception = onException;

	SetTimeout(responseHandler);
	Network::Session::AsyncSend(packet);
}

void AsyncSession::SetTimeout(const std::shared_ptr<IResponseHandler>& responseHandler)
{
	auto self = shared_from_this();
	Dispatch([this, self, responseHandler]() {
		if (true == response_handlers.empty())
		{
			expire_timer.AutoReset(true);
			expire_timer.SetTimer(1000, std::bind(&AsyncSession::OnTimeout, this));
		}

		response_handlers[responseHandler->msg_seq] = responseHandler;
	});
}

void AsyncSession::OnTimeout()
{
	auto self = shared_from_this();
	Dispatch([this, self]() {
		time_t now = time(nullptr);
		std::list<uint64_t> expires;
		for (auto& itr : response_handlers)
		{
			const std::shared_ptr<IResponseHandler>& responseHandler = itr.second;
			if (responseHandler->expire_time < now)
			{
				responseHandler->on_exception(Exception(ErrorCode::ResponseTimeoutError));
				expires.push_back(itr.first);
			}
		}

		for (uint32_t seq : expires)
		{
			response_handlers.erase(seq);
		}

		if (true == response_handlers.empty())
		{
			expire_timer.Cancel();
		}
	});
}

const std::shared_ptr<IResponseHandler> AsyncSession::FindResponseHandler(uint32_t seq)
{
	auto itr = response_handlers.find(seq);
	if (response_handlers.end() == itr)
	{
		return nullptr;
	}

	std::shared_ptr<IResponseHandler> responseHandler = itr->second;
	response_handlers.erase(seq);
	if (true == response_handlers.empty())
	{
		expire_timer.Cancel();
	}
	return responseHandler;
}

void AsyncSession::OnRead(const std::shared_ptr<Buffer>& buffer)
{
	while (0 < buffer->Size())
	{
		size_t readSize = std::min(buffer->Size(), recv_packet->Available());
		recv_packet->Append(buffer->ReadPtr(), readSize);
		buffer->Remove(readSize);

		while (Tcp::Packet::HEADER_SIZE <= (int)recv_packet->Size())
		{
			recv_packet->ReadHeader();
			if (Tcp::Packet::HEADER_SIZE > recv_packet->length)
			{
				throw GAMNET_EXCEPTION(ErrorCode::BufferUnderflowError, "buffer underflow(read size:", recv_packet->length, ")");
			}

			if (recv_packet->length >= (uint16_t)recv_packet->Capacity())
			{
				throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "buffer overflow(read size:", recv_packet->length, ")");
			}

			if (recv_packet->length > (uint16_t)recv_packet->Size())
			{
				break;
			}

			std::shared_ptr<Tcp::Packet> packet = recv_packet;
			auto self = shared_from_this();

			//////////////////////////////////////////////////////////////////////////
			MsgRouter_SendMsg_Ntf ntf;
			if (false == Network::Tcp::Packet::Load(ntf, packet))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "router message format error");
			}

			std::shared_ptr<Network::Tcp::Packet> buffer = Network::Tcp::Packet::Create();
			if (nullptr == buffer)
			{
				throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create packet");
			}

			buffer->Append(ntf.buffer.data(), ntf.buffer.size());
			buffer->ReadHeader();
			buffer->msg_seq = packet->msg_seq;

			if (0 == buffer->msg_seq)
			{
				return;
			}

			const std::shared_ptr<IResponseHandler> responseHandler = FindResponseHandler(packet->msg_seq);
			if (nullptr == responseHandler)
			{
				return;
			}
			responseHandler->OnReceive(buffer);

			recv_packet = Tcp::Packet::Create();
			if (nullptr == recv_packet)
			{
				throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create Packet instance");
			}
			packet->Remove(packet->length);
			recv_packet->Append(packet->ReadPtr(), packet->Size());

		}
	}
}

void AsyncSession::Close(int reason)
{
	auto self = shared_from_this();
	Dispatch([this, self]() {
		if(nullptr == socket)
		{
			return;
		}

		for (auto& itr : response_handlers)
		{
			const std::shared_ptr<IResponseHandler>& responseHandler = itr.second;
			responseHandler->on_exception(Exception(ErrorCode::SendMsgFailError));
		}

		response_handlers.clear();
		expire_timer.Cancel();
		socket = nullptr;
	});
}

}}} /* namespace Gamnet */
