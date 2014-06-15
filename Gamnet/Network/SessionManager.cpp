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
		for(auto itr : this->mapSession_)
		{
			if(itr.second->lastHeartBeatTime_ + keepAliveSec_ < time(NULL))
			{
				Log::Write(GAMNET_ERR, "kick out session(session_key:", itr.second->sessionKey_, ")");
				itr.second->OnError(ETIMEDOUT);
			}
		}
		this->timer_.Resume();
		Log::Write(GAMNET_INF, "[NETWORK] running session count : ", this->mapSession_.size());
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
	std::lock_guard<std::recursive_mutex> lo(lock_);
	mapSession_.erase(key);
	session->sessionKey_ = 0;
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
