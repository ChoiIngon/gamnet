#include "Session.h"
#include "Link.h"
#include "LinkManager.h"
#include "../Library/MD5.h"
#include "../Library/Random.h"
#include <list>

namespace Gamnet { namespace Network {

std::string Session::GenerateSessionToken(uint32_t session_key)
{
	return md5(Format(session_key, time(nullptr), Random::Range(1, 99999999)));
}

Session::Session() :
	session_key(0),
	session_token(""),
	expire_time(0),
	remote_address(nullptr),
	link(nullptr)
{
}

Session::~Session()
{
}

bool Session::Init()
{
	session_key = ++Network::SessionManager::session_key;
	session_token = "";
	expire_time = ::time(nullptr);
	link = nullptr;
	remote_address = nullptr;
	handler_container.Init();
	return true;
}

void Session::AsyncSend(const std::shared_ptr<Buffer>& buffer)
{
	std::shared_ptr<Link> _link = link;
	if(nullptr == _link)
	{
		LOG(ERR, "invalid link[session_key:", session_key, "]");
		return;
	}
	_link->AsyncSend(buffer);
}
void Session::AsyncSend(const char* data, int length)
{
	std::shared_ptr<Link> _link = link;
	if (nullptr == _link)
	{
		LOG(ERR, "invalid link[session_key:", session_key, "]");
		return;
	}
	_link->AsyncSend(data, length);
}
int Session::SyncSend(const std::shared_ptr<Buffer>& buffer)
{
	std::shared_ptr<Link> _link = link;
	if (nullptr == _link)
	{
		return -1;
	}
	return _link->SyncSend(buffer);
}
int Session::SyncSend(const char* data, int length)
{
	std::shared_ptr<Link> _link = link;
	if (nullptr == _link)
	{
		return -1;
	}
	return _link->SyncSend(data, length);
}

std::atomic<uint32_t> SessionManager::session_key;

SessionManager::SessionManager() : keepalive_time(0)
{
}

SessionManager::~SessionManager()
{
}

bool SessionManager::Init(int keepAliveSeconds)
{
	{
		std::lock_guard<std::mutex> lo(_lock);
		_sessions.clear();
		_timer.Cancel();
	}

	if(0 == keepAliveSeconds)
	{
		return true;
	}
	keepalive_time = keepAliveSeconds;
	_timer.AutoReset(true);
	if (false == _timer.SetTimer(5000, [this](){
		std::list<std::shared_ptr<Session>> sessions;
		time_t now_ = time(nullptr);

		std::lock_guard<std::mutex> lo(_lock);

		for(auto itr = _sessions.begin(); itr != _sessions.end();) {
			std::shared_ptr<Session> session = itr->second;
			if(session->expire_time + keepalive_time < now_)
			{
				sessions.push_back(session);
				_sessions.erase(itr++);
			}
	 		else { 
				++itr; 
			}
		}
		
		for(auto session : sessions)
		{
			LOG(GAMNET_ERR, "[session_key:", session->session_key, "] destroy idle session");
			std::shared_ptr<Link> link = session->link;
			if(nullptr != link)
			{
				link->Close(ErrorCode::IdleTimeoutError);
			}
			session->OnDestroy();	
		}
	}))
	{
		LOG(GAMNET_ERR, "session time out timer init fail");
		return false;
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
}} /* namespace Gamnet */
