#include "SessionManager.h"

namespace Gamnet { namespace Network {

SessionManager::SessionManager() : keepAliveSec_(0)
{
}

SessionManager::~SessionManager()
{
}

bool SessionManager::Init(int nKeepAliveSec)
{
	keepAliveSec_ = nKeepAliveSec;
	if(0 == keepAliveSec_)
	{
		return true;
	}
	if (false == timer_.SetTimer(keepAliveSec_ * 1000, [this](){
		std::lock_guard<std::recursive_mutex> lo(this->lock_);
		time_t now_ = time(NULL);
		for(auto itr = this->mapSession_.begin(); itr != this->mapSession_.end();) {
			if(itr->second->lastHeartBeatTime_ + keepAliveSec_ < now_)
			{
				std::shared_ptr<Session> session = itr->second;
				Log::Write(GAMNET_ERR, "idle session timeout(ip:", session->remote_address_.to_string(), ", session_key:", session->sessionKey_,")");

		        this->mapSession_.erase(itr++);
		        session->sessionKey_ = 0;
		        session->strand_.wrap(std::bind(&Session::OnError, session, ETIMEDOUT))();
		    }
		    else {
		        ++itr;
		    }
		}
		this->timer_.Resume();
	}))
	{
		Log::Write(GAMNET_ERR, "session time out timer init fail");
		return false;
	}
	return true;
}

bool SessionManager::AddSession(uint64_t key, std::shared_ptr<Session> pSession)
{
	std::lock_guard<std::recursive_mutex> lo(lock_);
	if(false == mapSession_.insert(std::make_pair(key, pSession)).second)
	{
		return false;
	}
	return true;
}

void SessionManager::DelSession(uint64_t key, std::shared_ptr<Session> session)
{
	if(0 == key)
	{
		return;
	}
	std::lock_guard<std::recursive_mutex> lo(lock_);
	mapSession_.erase(key);
}

std::shared_ptr<Session> SessionManager::FindSession(uint64_t key)
{
	std::lock_guard<std::recursive_mutex> lo(lock_);
	auto itr = mapSession_.find(key);
	if(mapSession_.end() == itr)
	{
		return NULL;
	}
	return itr->second;
}

size_t SessionManager::Size()
{
	std::lock_guard<std::recursive_mutex> lo(lock_);
	return mapSession_.size();
}

}};
