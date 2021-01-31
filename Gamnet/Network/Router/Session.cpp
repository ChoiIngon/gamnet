#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "SessionManager.h"
#include "RouterCaster.h"
#include <boost/bind.hpp>

namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_context& io_context = Singleton<boost::asio::io_context>::GetInstance();

Session::Session()
	: Network::Tcp::Session()
{
}

Session::~Session()
{
}

bool Session::Init()
{
	if (false == Network::Tcp::Session::Init())
	{
		return false;
	}
	
	return true;
}

void Session::OnCreate()
{
}

void Session::OnAccept()
{
	auto self = std::static_pointer_cast<Session>(shared_from_this());
	if (false == Singleton<RouterCaster>::GetInstance().RegisterAddress(router_address, self))
	{
		throw GAMNET_EXCEPTION(ErrorCode::DuplicateRouterAddress, 
			"duplicate router server(",
			"router_address:", router_address.ToString(), 
			", ip:", remote_endpoint.address().to_v4().to_string(), ", port:", remote_endpoint.port(), 
			")"
		);
	}
	static_cast<SessionManager*>(session_manager)->on_connect(router_address);
	session_state = Tcp::Session::Session::State::AfterAccept;
}

void Session::OnClose(int reason)
{
	if (Tcp::Session::Session::State::AfterAccept == session_state)
	{
		static_cast<SessionManager*>(session_manager)->on_close(router_address);
		Singleton<RouterCaster>::GetInstance().UnregisterAddress(router_address);
	}
}

void Session::OnDestroy()
{
}

void Session::Close(int reason)
{
	auto self = shared_from_this();
	Dispatch([this, self, reason]() {
		if (nullptr == socket)
		{
			return;
		}

		for (auto& itr : async_responses)
		{
			const std::shared_ptr<IAsyncResponse>& response = itr.second;
			response->OnException(Exception(ErrorCode::SendMsgFailError));
		}

		async_responses.clear();

		OnClose(reason);
		socket = nullptr;
		OnDestroy();
		session_manager->Remove(shared_from_this());
	});
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
	Network::Session::AsyncSend(packet);
}

void Session::AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, const std::shared_ptr<IAsyncResponse>& response)
{
	auto self = std::static_pointer_cast<Session>(shared_from_this());
	response->session = self;
	response->StartTimer([this, self, response]() {
		async_responses.erase(response->seq);
	});
	
	Dispatch([this, self, response]() {
		async_responses[response->seq] = response;
	});

	AsyncSend(packet);
}

LocalSession::LocalSession()
{
}

LocalSession::~LocalSession()
{
}

void LocalSession::AsyncSend(const std::shared_ptr<Tcp::Packet>& packet)
{
	auto self = shared_from_this();
	try {
		Dispatch(std::bind(&Network::SessionManager::OnReceive, self->session_manager, self, packet));
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
	}
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

}}} /* namespace Gamnet */
