#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "SessionManager.h"
#include "RouterCaster.h"
#include <boost/bind.hpp>
#include <future>
#include <boost/asio/socket_base.hpp>

namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Socket::Socket() 
{
	connector.connect_handler = std::bind(&Socket::OnConnect, this, std::placeholders::_1);
}

Socket::~Socket() 
{
}

bool Socket::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	remote_endpoint = endpoint;
	return connector.SyncConnect(remote_endpoint.address().to_v4().to_string(), remote_endpoint.port(), 5);
}

bool Socket::Reconnect()
{
	return Connect(remote_endpoint);
}

void Socket::OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
{
	this->socket = socket;

	this->remote_endpoint = this->socket->remote_endpoint();

	MsgRouter_RegisterAddress_Ntf ntf;
	ntf.router_address = Singleton<SessionManager>::GetInstance().local_address;

	std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
	packet->Write(ntf);
	SyncSend(packet);
}

void Socket::Close(int reason)
{
	strand->dispatch([=](){
		this->socket = nullptr;
	});
}

std::shared_ptr<Tcp::Packet> Socket::SyncRead()
{
	std::promise<std::shared_ptr<Tcp::Packet>> promise;
	// promise 써줘야 함. 다른 스레드에서 호출 됨.
	auto self = shared_from_this();
	strand->dispatch([self, &promise](){
		std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
		do {
			if(nullptr == self->socket)
			{
				promise.set_value(nullptr);
				return;
			}
			if(0 == packet->Available())
			{
				throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "buffer overflow(read size:", packet->length, ")");
			}

			try {
				int readBytes = self->socket->read_some(boost::asio::buffer(packet->WritePtr(), packet->Available()));
				if(0 == readBytes)
				{
					throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "buffer overflow(read size:", packet->length, ")");
				}
				packet->write_index += readBytes;
			}
			catch(const boost::system::system_error& e)
			{
				self->Close(ErrorCode::Success);
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
	return promise.get_future().get();
}

Session::Session() 
	: Network::Tcp::Session()
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

void Session::OnConnect()
{	
	static_cast<SessionManager*>(session_manager)->on_connect(router_address);
}

void Session::OnClose(int reason)
{
	//LOG(INF, "[", session_key, "] remote server closed(ip:", GetRemoteAddress().to_string(), ", service_name:", address.service_name, ", reason:", reason, ")");
	if("" != router_address.service_name)
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
	std::shared_ptr<Socket> session = pool.Create();
	if(nullptr == session->socket)
	{
		if(nullptr == this->socket)
		{
			return nullptr;
		}
		session->Connect(this->socket->remote_endpoint());
	}
	session->SyncSend(packet);
	return session->SyncRead();
}

void LocalSession::AsyncSend(const std::shared_ptr<Tcp::Packet> packet)
{
	auto self(shared_from_this());
	strand->post(boost::bind(&Network::SessionManager::OnReceive, session_manager, self, packet));
}


}}} /* namespace Gamnet */
