#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "SessionManager.h"
#include "RouterCaster.h"
#include <boost/bind.hpp>

namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Session::SyncSession::SyncSession() 
{
	connector.connect_handler = std::bind(&SyncSession::OnConnect, this, std::placeholders::_1);
}

Session::SyncSession::~SyncSession() 
{
}

bool Session::SyncSession::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	remote_endpoint = endpoint;
	return connector.SyncConnect(remote_endpoint.address().to_v4().to_string(), remote_endpoint.port(), 5);
}

void Session::SyncSession::OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
	this->socket = socket;
	this->remote_endpoint = socket->remote_endpoint();

	MsgRouter_RegisterAddress_Ntf ntf;
	ntf.router_address = Singleton<SessionManager>::GetInstance().local_address;

	std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
	packet->Write(ntf);
	SyncSend(packet);
}

void Session::SyncSession::Close(int reason)
{
	strand->dispatch([this](){
		expire_timer.Cancel();
		socket = nullptr;
	});
}

std::shared_ptr<Tcp::Packet> Session::SyncSession::SyncRead(int timeout)
{
	auto self = shared_from_this();
	std::promise<std::shared_ptr<Tcp::Packet>> promise;
	
	// promise 써줘야 함. 다른 스레드에서 호출 됨.
	strand->dispatch([this, &promise, timeout](){
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
		promise.set_value(packet);
	});
	expire_timer.Cancel();
	return promise.get_future().get();
}

Session::Session()
	: Network::Tcp::Session()
	, master(false)
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
	if(false == Network::Tcp::Session::Init())
	{
		return false;
	}
	master = false;
}

void Session::OnConnect()
{	
	static_cast<SessionManager*>(session_manager)->on_connect(router_address);
}

void Session::OnClose(int reason)
{
	if(true == master)
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
	if(nullptr == socket)
	{
		return;
	}

	OnClose(reason);
	socket = nullptr;

	OnDestroy();
	session_manager->Remove(shared_from_this());
}
/*
const std::shared_ptr<Session::ResponseTimeout> Session::FindResponseTimeout(uint32_t msgSEQ)
{
	std::shared_ptr<ResponseTimeout> timeout = nullptr;
	strand->dispatch([this, msgSEQ, &timeout](){
		auto itr = response_timeouts.find(msgSEQ);
		if (response_timeouts.end() != itr)
		{
			timeout = itr->second;
			response_timeouts.erase(msgSEQ);
		}
		if (0 == response_timeouts.size())
		{
			expire_timer.Cancel();
		}
	});
	return timeout;
}

void Session::OnResponseTimeout()
{
	time_t now = time(nullptr);
	std::list<uint64_t> expiredMsgSEQs;
	for(auto& itr : response_timeouts)
	{
		const std::shared_ptr<ResponseTimeout>& timeout = itr.second;
		if(timeout->expire_time < now)
		{
			uint32_t msgSEQ = itr.first;
			timeout->on_timeout();
			handler_container.Find(msgSEQ);
			expiredMsgSEQs.push_back(msgSEQ);
		}
	}

	for(uint64_t msgSEQ : expiredMsgSEQs)
	{
		response_timeouts.erase(msgSEQ);
	}

	if(0 == response_timeouts.size())
	{
		expire_timer.Cancel();
	}
}
*/
std::shared_ptr<Tcp::Packet> Session::SyncSend(const std::shared_ptr<Tcp::Packet>& packet, int timeout)
{
	std::shared_ptr<SyncSession> session = syncsession_pool.Create();
	if (nullptr == session->socket)
	{
		if(false ==	session->Connect(remote_endpoint))
		{
			return nullptr;
		}
	}

	if(0 > session->SyncSend(packet))
	{
		return nullptr;
	}
	return session->SyncRead(timeout);
}

void LocalSession::AsyncSend(const std::shared_ptr<Tcp::Packet> packet)
{
	auto self(shared_from_this());
	strand->post(boost::bind(&Network::SessionManager::OnReceive, session_manager, self, packet));
}


}}} /* namespace Gamnet */
