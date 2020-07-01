#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "SessionManager.h"
#include "RouterCaster.h"
#include <boost/bind.hpp>

namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

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

void Session::AsyncSend(const std::shared_ptr<Tcp::Packet> packet, uint32_t responseMsgID, std::shared_ptr<IHandler> handler, int seconds, std::function<void()> onTimeout)
{
	handler_container.Register(packet->msg_seq, handler);

	std::shared_ptr<ResponseTimeout> timeout = std::make_shared<ResponseTimeout>();
	timeout->expire_time = time(nullptr) + seconds;
	timeout->on_timeout = onTimeout;

	strand->dispatch([=](){
		response_timeouts[packet->msg_seq] = timeout;
		if(1 == response_timeouts.size())
		{
			expire_timer.AutoReset(true);
			expire_timer.SetTimer(1000, strand->wrap(std::bind(&Session::OnResponseTimeout, this)));
		}
	});
	Network::Session::AsyncSend(packet);
}

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

void LocalSession::AsyncSend(const std::shared_ptr<Tcp::Packet> packet)
{
	auto self(shared_from_this());
	strand->post(boost::bind(&Network::SessionManager::OnReceive, session_manager, self, packet));
}
}}} /* namespace Gamnet */
