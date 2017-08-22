/*
 * Session.cpp
 *
 *  Created on: 2017. 8. 20.
 *      Author: kukuta
 */

#include "Session.h"
#include "Link.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network {

Session::Session() : expire_time(0), session_key(""), link(std::shared_ptr<Link>(NULL)), manager(NULL), remote_address(NULL)
{
}

Session::~Session()
{
}

void Session::AsyncSend(const std::shared_ptr<Buffer>& buffer)
{
	std::shared_ptr<Network::Link> _link = link.lock();
	_link->AsyncSend(buffer);
}
void Session::AsyncSend(const char* data, size_t length)
{
	std::shared_ptr<Network::Link> _link = link.lock();
	_link->AsyncSend(data, length);
}
int Session::SyncSend(const std::shared_ptr<Buffer>& buffer)
{
	std::shared_ptr<Network::Link> _link = link.lock();
	return _link->SyncSend(buffer);
}
int Session::SyncSend(const char* data, size_t length)
{
	std::shared_ptr<Network::Link> _link = link.lock();
	return _link->SyncSend(data, length);
}

SessionManager::SessionManager() : _keepalive_time(0)
{
}

SessionManager::~SessionManager()
{
}

bool SessionManager::Init(int keepAliveSeconds)
{
	if(0 == keepAliveSeconds)
	{
		return true;
	}
	_keepalive_time = keepAliveSeconds;
	if (false == _timer.SetTimer(5000, [&](){
		std::lock_guard<std::recursive_mutex> lo(_lock);
		time_t now_ = time(NULL);
		for(auto itr = _sessions.begin(); itr != _sessions.end();) {
			std::shared_ptr<Session> session = itr->second;
			if(session->expire_time  < now_)
			{
				LOG(GAMNET_ERR, "idle session timeout(session_key:", session->session_key,")");
		        _sessions.erase(itr++);
		    }
		    else {
		        ++itr;
		    }
		}
		_timer.Resume();
	}))
	{
		LOG(GAMNET_ERR, "session time out timer init fail");
		return false;
	}
	return true;
}

bool SessionManager::Add(const std::string& key, const std::shared_ptr<Session>& session)
{
	std::lock_guard<std::recursive_mutex> lo(_lock);
	if(false == _sessions.insert(std::make_pair(key, session)).second)
	{
		LOG(GAMNET_ERR, "duplicated session key(session_key:", key, ")");
		return false;
	}
	session->expire_time = time(NULL) + _keepalive_time;
	return true;
}

void SessionManager::Remove(const std::string& key)
{
	if("" == key)
	{
		return;
	}
	std::lock_guard<std::recursive_mutex> lo(_lock);
	_sessions.erase(key);
}

std::shared_ptr<Session> SessionManager::Find(const std::string& key)
{
	std::lock_guard<std::recursive_mutex> lo(_lock);
	auto itr = _sessions.find(key);
	if(_sessions.end() == itr)
	{
		return NULL;
	}
	return itr->second;
}

size_t SessionManager::Size()
{
	std::lock_guard<std::recursive_mutex> lo(_lock);
	return _sessions.size();
}
}} /* namespace Gamnet */
