#include "Session.h"
#include "../../Log/Log.h"
#include "../Tcp/Tcp.h"
#include "SessionManager.h"
#include "RouterCaster.h"
#include <boost/bind.hpp>

namespace Gamnet { namespace Network { namespace Router {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Session::AnswerWatingSessionManager::AnswerWatingSessionManager() : timer(Time::Timer::Create())
{
}

Session::AnswerWatingSessionManager::~AnswerWatingSessionManager()
{
}

void Session::AnswerWatingSessionManager::Init()
{
	Clear();
	timer->AutoReset(true);
	timer->SetTimer(10000, [this]() {
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
	timer->Cancel();
}

Session::Session() 
	: Network::Tcp::Session()
	, send_session(false)
{
}

Session::~Session() 
{
}

void Session::OnCreate() 
{
	if(true == send_session)
	{
		wait_session_manager.Init();
	}
}

void Session::OnAccept() 
{
}

void Session::OnConnect()
{	
	if(true == send_session)
	{
		static_cast<SessionManager*>(session_manager)->on_connect(router_address);
	}
}

void Session::OnClose(int reason)
{
	//LOG(INF, "[", session_key, "] remote server closed(ip:", GetRemoteAddress().to_string(), ", service_name:", address.service_name, ", reason:", reason, ")");
	if("" != router_address.service_name)
	{
		if(true == send_session)
		{
			Singleton<RouterCaster>::GetInstance().UnregisterAddress(router_address);
			static_cast<SessionManager*>(session_manager)->on_close(router_address);
		}
	}
	wait_session_manager.Clear();
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

void LocalSession::AsyncSend(const std::shared_ptr<Tcp::Packet> packet)
{
	auto self(shared_from_this());
	strand->wrap(boost::bind(&Network::SessionManager::OnReceive, session_manager, self, packet))();
}
}}} /* namespace Gamnet */
