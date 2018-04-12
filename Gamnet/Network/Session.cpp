#include "Session.h"
#include "Link.h"
#include "LinkManager.h"
#include "../Library/MD5.h"
#include "../Library/Random.h"
#include "../Library/Timer.h"
#include <list>

namespace Gamnet { namespace Network {

static boost::asio::io_service& GetIOService()
{
	static boost::asio::io_service io_service_;
	return io_service_;
}

static std::vector<std::thread > workers_;

void Session::CreateWorkerThreadPool(uint32_t threadCount)
{
	for (int i = 0; i<threadCount + 1; i++)
	{
		workers_.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &GetIOService())));
	}
}

std::string Session::GenerateSessionToken(uint32_t session_key)
{
	return md5(Format(session_key, time(nullptr), Random::Range(1, 99999999)));
}

Session::Session() :
	session_key(0),
	session_token(""),
	expire_time(0),
	remote_address(nullptr),
	strand(GetIOService()),
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

bool Session::AsyncSend(const std::shared_ptr<Buffer>& buffer)
{
	std::shared_ptr<Link> _link = link;
	if(nullptr == _link)
	{
		LOG(ERR, "invalid link[session_key:", session_key, "]");
		return false;
	}
	_link->AsyncSend(buffer);
	return true;
}
bool Session::AsyncSend(const char* data, int length)
{
	std::shared_ptr<Link> _link = link;
	if (nullptr == _link)
	{
		LOG(ERR, "invalid link[session_key:", session_key, "]");
		return false;
	}
	_link->AsyncSend(data, length);
	return true;
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

void Session::AttachLink(const std::shared_ptr<Link>& link)
{
	if(nullptr != this->link)
	{
	//	this->link->session = nullptr;
		this->remote_address = nullptr;
		this->link = nullptr;
	}

	if(nullptr != link)
	{
	//	link->session = shared_from_this();
		this->link = link;
		this->remote_address = &(link->remote_address);
	}
}

std::atomic<uint32_t> SessionManager::session_key;

SessionManager::SessionManager() : 
	_keepalive_time(0),
	_deadline_timer(GetIOService())
{
}

SessionManager::~SessionManager()
{
}

bool SessionManager::Init(int keepAliveSeconds)
{
	std::lock_guard<std::mutex> lo(_lock);

	_keepalive_time = keepAliveSeconds;
	_deadline_timer.cancel();
	_deadline_timer.expires_from_now(boost::posix_time::seconds((0 == _keepalive_time ? 3600 : _keepalive_time)));
	_deadline_timer.async_wait(boost::bind(&SessionManager::OnTimerExpire, this, boost::asio::placeholders::error));
	
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

void SessionManager::OnTimerExpire(const boost::system::error_code& ec)
{
	std::list<std::shared_ptr<Session>> sessionsToBeDeleted;
	{
		std::lock_guard<std::mutex> lo(_lock);
		time_t now_ = time(nullptr);
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
		LOG(GAMNET_ERR, "[session_key:", session->session_key, "] destroy idle session");
		std::shared_ptr<Link> link = session->link;
		if (nullptr != link)
		{
			link->Close(ErrorCode::IdleTimeoutError);
		}
		session->OnDestroy();
	}

	_deadline_timer.expires_from_now(boost::posix_time::seconds((0 == _keepalive_time ? 3600 : _keepalive_time)));
	_deadline_timer.async_wait(boost::bind(&SessionManager::OnTimerExpire, this, boost::asio::placeholders::error));
}

}} /* namespace Gamnet */
