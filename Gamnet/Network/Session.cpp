#include "Session.h"
#include "Link.h"
#include "LinkManager.h"
#include "../Library/MD5.h"
#include "../Library/Random.h"
#include "../Library/Timer.h"
#include <list>

namespace Gamnet { namespace Network {
	

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

static std::atomic<uint32_t> SESSION_KEY;

std::string Session::GenerateSessionToken(uint32_t session_key)
{
	return md5(Format(session_key, time(nullptr), Random::Range(1, 99999999)));
}


Session::Session() :
	session_key(0),
	session_token(""),
	link(nullptr)
{
}

Session::~Session()
{
}

bool Session::Init()
{
	session_key = ++SESSION_KEY;
	return true;
}

void Session::Clear()
{
	session_key = 0;
	session_token = "";
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

SessionManager::SessionManager() 
{
}

SessionManager::~SessionManager()
{
}

bool SessionManager::Init()
{
	std::lock_guard<std::mutex> lo(_lock);
	_sessions.clear();
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

const boost::asio::ip::address& Session::GetRemoteAddress() const
{
	assert(nullptr != link);
	return link->remote_address;
}
}} /* namespace Gamnet */
