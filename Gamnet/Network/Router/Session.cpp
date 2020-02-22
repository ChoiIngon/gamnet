#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "LinkManager.h"
#include "RouterCaster.h"


namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Session::AnswerWatingSessionManager::AnswerWatingSessionManager()
{
}

Session::AnswerWatingSessionManager::~AnswerWatingSessionManager()
{
}

void Session::AnswerWatingSessionManager::Init()
{
	Clear();
	timer_.AutoReset(true);
	timer_.SetTimer(10000, [this]() {
		std::lock_guard<std::mutex> lo(this->lock_);
		time_t now_ = time(nullptr);
		for (auto itr = this->wait_sessions_.begin(); itr != this->wait_sessions_.end();) {
			std::pair<time_t, std::shared_ptr<Network::Session>> pair_ = itr->second;
			if (pair_.first + 5 < now_)
			{
				Log::Write(GAMNET_ERR, "[Router] idle session timeout");
				this->wait_sessions_.erase(itr++);
			}
			else {
				++itr;
			}
		}
	});
}

bool Session::AnswerWatingSessionManager::AddSession(uint64_t recv_seq, std::shared_ptr<Network::Tcp::Session> session)
{
	std::lock_guard<std::mutex> lo(lock_);
	return wait_sessions_.insert(std::make_pair(recv_seq, std::make_pair(time(nullptr), session))).second;
}

std::shared_ptr<Network::Tcp::Session> Session::AnswerWatingSessionManager::FindSession(uint64_t recv_seq)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto itr = wait_sessions_.find(recv_seq);
	if (wait_sessions_.end() == itr)
	{
		return nullptr;
	}
	std::shared_ptr<Network::Tcp::Session> session = itr->second.second;
	wait_sessions_.erase(itr);
	return session;
}

void Session::AnswerWatingSessionManager::Clear()
{
	std::lock_guard<std::mutex> lo(lock_);
	wait_sessions_.clear();
	timer_.Cancel();
}

Session::Session() : Network::Tcp::Session() 
{
	onRouterConnect = [](const Address&) {};
	onRouterClose = [](const Address&) {};
}

Session::~Session() 
{
}

void Session::OnCreate() 
{
	watingSessionManager_.Init();
}

void Session::OnAccept() 
{
	onRouterClose = LinkManager::onRouterClose;
}

void Session::OnConnect()
{	
	LOG(GAMNET_INF, "[Router] [", link->link_key, "/", session_key, "/", Singleton<LinkManager>::GetInstance().local_address.service_name.c_str(), "] connect success..(remote ip:", GetRemoteAddress().to_string(), ")");
	MsgRouter_SetAddress_Req req;
	req.router_address = Singleton<LinkManager>::GetInstance().local_address;
	Network::Tcp::SendMsg(std::static_pointer_cast<Session>(shared_from_this()), req, false);
	LOG(GAMNET_INF, "[Router] [", link->link_key, "/", session_key, "/", Singleton<LinkManager>::GetInstance().local_address.service_name.c_str(),"] localhost->", GetRemoteAddress().to_string(), " send MsgRouter_SetAddress_Req(", req.router_address.service_name, ":", ToString<ROUTER_CAST_TYPE>(req.router_address.cast_type), ":", req.router_address.id, ")");
}

void Session::OnClose(int reason)
{
	LOG(GAMNET_INF, "[Router] remote server closed(session_key:", session_key, ", ip:", GetRemoteAddress().to_string(), ", service_name:", address.service_name, ", reason:", reason, ")");
	if("" != address.service_name)
	{
		onRouterClose(address);
		Singleton<RouterCaster>::GetInstance().UnregisterAddress(address);
	}
	watingSessionManager_.Clear();
}

void Session::OnDestroy()
{
}

}}} /* namespace Gamnet */
