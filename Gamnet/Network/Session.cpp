#include "Session.h"
#include "Link.h"
#include "LinkManager.h"
#include "../Library/MD5.h"
#include "../Library/Random.h"
#include "../Library/Timer.h"
#include <list>

namespace Gamnet { namespace Network {
	

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

std::atomic<uint32_t> Session::session_key_generator;

std::string Session::GenerateSessionToken(uint32_t session_key)
{
	return md5(Format(session_key, time(nullptr), Random::Range(1, 99999999)));
}


Session::Session() :
	session_key(0),
	session_token(""),
	expire_time(0),
	link(nullptr)
{
}

Session::~Session()
{
}

bool Session::Init()
{
	session_key = ++Session::session_key_generator;
	expire_time = ::time(nullptr);
	return true;
}

void Session::Clear()
{
	session_key = 0;
	session_token = "";
	expire_time = 0;
	link = nullptr;
	handler_container.Init();
}

bool Session::AsyncSend(const std::shared_ptr<Buffer>& buffer)
{
	if(nullptr == link)
	{
		LOG(ERR, "invalid link[session_key:", session_key, "]");
		return false;
	}
	link->AsyncSend(buffer);
	return true;
}

bool Session::AsyncSend(const char* data, int length)
{
	if (nullptr == link)
	{
		LOG(ERR, "invalid link[session_key:", session_key, "]");
		return false;
	}
	link->AsyncSend(data, length);
	return true;
}
int Session::SyncSend(const std::shared_ptr<Buffer>& buffer)
{
	if (nullptr == link)
	{
		return -1;
	}
	return link->SyncSend(buffer);
}
int Session::SyncSend(const char* data, int length)
{
	if (nullptr == link)
	{
		return -1;
	}
	return link->SyncSend(data, length);
}

void Session::AttachLink(const std::shared_ptr<Link>& link)
{
	this->link = link;
}

SessionManager::SessionManager() : 
	_keepalive_time(0) 
{
}

SessionManager::~SessionManager()
{
}

bool SessionManager::Init(int keepAliveSeconds)
{
	std::lock_guard<std::mutex> lo(_lock);

	_keepalive_time = keepAliveSeconds;

	_sessions.clear();
	_timer.Cancel();
	_timer.AutoReset(true);
	if(0 < _keepalive_time)
	{
		_timer.SetTimer(_keepalive_time * 1000, std::bind(&SessionManager::OnTimerExpire, this));
	}
	
	return true;
}

bool SessionManager::Add(uint32_t key, const std::shared_ptr<Session>& session)
{
	std::lock_guard<std::mutex> lo(_lock);
	if(false == _sessions.insert(std::make_pair(key, session)).second)
	{
		LOG(GAMNET_ERR, "[link_key:", session->link->link_key, ", session_key:", key, "] duplicated session key");
		assert(!"duplicate session");
		return false;
	}
	
	return true;
}

void SessionManager::Remove(uint32_t key)
{
	std::lock_guard<std::mutex> lo(_lock);
	_sessions.erase(key);
}

std::shared_ptr<Session> SessionManager::Find(uint32_t key)
{
	std::lock_guard<std::mutex> lo(_lock);
	auto itr = _sessions.find(key);
	if(_sessions.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}

size_t SessionManager::Size()
{
	std::lock_guard<std::mutex> lo(_lock);
	return _sessions.size();
}
/*
void SessionManager::Flush()
{
	std::list<std::shared_ptr<Session>> sessionsToBeDeleted;
	{
		std::lock_guard<std::mutex> lo(_lock);
		LOG(WRN, "flush invalid sessions(", _sessions.size(), ")");
		for (auto itr = _sessions.begin(); itr != _sessions.end();) 
		{
			std::shared_ptr<Session> session = itr->second;
			if (nullptr == session->link)
			{
				sessionsToBeDeleted.push_back(session);
				_sessions.erase(itr++);
			}
			else {
				++itr;
			}
		}
	}

	for (auto session : sessionsToBeDeleted)
	{
		LOG(GAMNET_ERR, "[session_key:", session->session_key, "] destroy idle session");
		std::lock_guard<std::recursive_mutex> lo(session->lock);
		session->OnDestroy();
		session->AttachLink(nullptr);
	}
}
*/
void SessionManager::OnTimerExpire()
{
	/*
	std::list<std::shared_ptr<Session>> sessionsToBeDeleted;
	{
		std::lock_guard<std::mutex> lo(_lock);
		int64_t now_ = time(nullptr);
		if(0 < _keepalive_time)
		{
			for (auto itr = _sessions.begin(); itr != _sessions.end();) {
				std::shared_ptr<Session> session = itr->second;
				if (session->expire_time + _keepalive_time < now_)
				{
					sessionsToBeDeleted.push_back(session);
					_sessions.erase(itr++);
				}
				else {
					++itr;
				}
			}
		}
	}

	for (auto session : sessionsToBeDeleted)
	{
		std::shared_ptr<Link> link = session->link;
		if(nullptr == link)
		{
			continue;
		}

		LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session->session_key, "] destroy idle session");
		link->strand.wrap([=]() {
			if (true == link->socket.is_open())
			{
				link->Close();
				return;
			}
			std::lock_guard<std::recursive_mutex> lo(session->lock);
			session->OnClose(ErrorCode::IdleTimeoutError);
			session->OnDestroy();
			session->link = nullptr;
			link->session = nullptr;
		});		
	}
	*/
}

const boost::asio::ip::address& Session::GetRemoteAddress() const
{
	assert(nullptr != link);
	return link->remote_address;
}
}} /* namespace Gamnet */
