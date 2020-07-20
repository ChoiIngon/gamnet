#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "SessionManager.h"
#include "RouterCaster.h"
#include <boost/bind.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/yield.hpp>

namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_context& io_context = Singleton<boost::asio::io_context>::GetInstance();

Session::Session()
	: Network::Tcp::Session()
	, type(TYPE::INVALID)
	, asyncsession_pool(65535, AsyncSession::Factory())
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
	timeout_seq = 0;
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
	std::shared_ptr<SyncSession> session = syncsession_pool.Create();
	if (nullptr == session->socket)
	{
		if (false == session->Connect(remote_endpoint))
		{
			return nullptr;
		}
	}
	/*
	boost::asio::spawn(*strand, [session, packet](boost::asio::yield_context context) {
		if (nullptr == session->socket)
		{
			return;
		}

		int totalSentBytes = 0;
		while (0 < packet->Size())
		{
			try {
				boost::system::error_code ec;
				int sentBytes = boost::asio::async_write(*(session->socket), boost::asio::buffer(packet->ReadPtr(), packet->Size()), context[ec]);
				if (0 > sentBytes || 0 != ec.value())
				{
					break;
				}
				if (0 == sentBytes)
				{
					break;
				}
				totalSentBytes += sentBytes;
				packet->Remove(sentBytes);
			}
			catch (const boost::system::system_error& e)
			{
				LOG(ERR, "send exception(errno:", errno, ", errstr:", e.what(), ")");
				session->Close(errno);
				break;
			}
		}
	});
	*/
	if (0 > session->SyncSend(packet))
	{
		return nullptr;
	}
	return session->SyncRead(timeout);
}

void Session::AsyncSend(const std::shared_ptr<Tcp::Packet>& packet)
{
	std::shared_ptr<AsyncSession> session = asyncsession_pool.Create();
	if (nullptr == session->socket)
	{
		if (false == session->Connect(remote_endpoint))
		{
			throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError);
		}
	}

	session->AsyncSend(packet);
}

void Session::AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, std::function<void(const std::shared_ptr<Tcp::Packet>&)> onReceive, std::function<void()> onTimeout, int seconds)
{
	std::shared_ptr<AsyncSession> session = asyncsession_pool.Create();
	if (nullptr == session->socket)
	{
		if (false == session->Connect(remote_endpoint))
		{
			throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError);
		}
	}

	session->AsyncSend(packet, onReceive, onTimeout, seconds);

}
SyncSession::SyncSession() 
{
	connector.connect_handler = std::bind(&SyncSession::OnConnect, this, std::placeholders::_1);
}

SyncSession::~SyncSession() 
{
}

bool SyncSession::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	return connector.SyncConnect(endpoint.address().to_v4().to_string(), endpoint.port(), 5);
}

void SyncSession::OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
	this->socket = socket;
	
	MsgRouter_RegisterAddress_Ntf ntf;
	ntf.router_address = Singleton<SessionManager>::GetInstance().local_address;

	std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
	packet->Write(ntf);
	SyncSend(packet);
}

void SyncSession::Close(int reason)
{
	boost::asio::dispatch(*strand, [this](){
		expire_timer.Cancel();
		socket = nullptr;
	});
}

std::shared_ptr<Tcp::Packet> SyncSession::SyncRead(int timeout)
{
	auto self = shared_from_this();
	std::promise<std::shared_ptr<Tcp::Packet>> promise;
	
	// promise 써줘야 함. 다른 스레드에서 호출 됨.
	boost::asio::dispatch(*strand, [this, &promise, timeout](){
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

AsyncSession* AsyncSession::Factory::operator()()
{
	AsyncSession* session = new AsyncSession();
	session->session_manager = &Singleton<SessionManager>::GetInstance();
	return session;
}

bool AsyncSession::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	return connector.SyncConnect(endpoint.address().to_v4().to_string(), endpoint.port(), 5);
}

void AsyncSession::OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
	this->socket = socket;
	type = Session::TYPE::SEND;

	AsyncRead();
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

AsyncSession::AsyncSession()
{
	connector.connect_handler = std::bind(&AsyncSession::OnConnect, this, std::placeholders::_1);
}

void AsyncSession::AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, std::function<void(const std::shared_ptr<Tcp::Packet>&)>& onReceive, std::function<void()>& onTimeout, int seconds)
{
	std::shared_ptr<Timeout> timeout = timeout_pool.Create();
	timeout->timeout_seq = packet->msg_seq;
	timeout->expire_time = time(nullptr) + seconds;
	timeout->on_receive = onReceive;
	timeout->on_timeout = onTimeout;
	boost::asio::dispatch(*strand, boost::bind(&AsyncSession::SetTimeout, this, timeout));
	Network::Session::AsyncSend(packet);
}

void AsyncSession::SetTimeout(const std::shared_ptr<Timeout>& timeout)
{
	if(true == timeouts.empty())
	{
		expire_timer.AutoReset(true);
		expire_timer.SetTimer(1000, boost::asio::bind_executor(*strand, boost::bind(&AsyncSession::OnTimeout, this)));
	}

	timeouts[timeout->timeout_seq] = timeout;
}

void AsyncSession::OnTimeout()
{
	time_t now = time(nullptr);
	std::list<uint64_t> expires;
	for (auto& itr : timeouts)
	{
		const std::shared_ptr<Timeout>& timeout = itr.second;
		if (timeout->expire_time < now)
		{
			timeout->on_timeout();
			expires.push_back(itr.first);
		}
	}

	for (uint32_t seq : expires)
	{
		timeouts.erase(seq);
	}

	if (true == timeouts.empty())
	{
		expire_timer.Cancel();
	}
}

const std::shared_ptr<Session::Timeout> AsyncSession::FindTimeout(uint32_t seq) 
{
	auto itr = timeouts.find(seq);
	if (timeouts.end() == itr)
	{
		return nullptr;
	}

	std::shared_ptr<Session::Timeout> timeout = itr->second;
	timeouts.erase(seq);
	if (true == timeouts.empty())
	{
		expire_timer.Cancel();
	}
	return timeout;
}

}}} /* namespace Gamnet */
