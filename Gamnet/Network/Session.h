#ifndef GAMNET_NETWORK_SESSION_H_
#define GAMNET_NETWORK_SESSION_H_

#include <boost/asio.hpp>
#include <atomic>
#include <map>

#include "../Library/Buffer.h"
#include "../Log/Log.h"
#include "HandlerContainer.h"

namespace Gamnet { namespace Network {

class Link;
class LinkManager;
class Session;

class SessionManager
{
	boost::asio::deadline_timer						_deadline_timer;
	std::mutex										_lock;
	std::map<uint32_t, std::shared_ptr<Session>>	_sessions;
	uint64_t										_keepalive_time;
public :
	static std::atomic<uint32_t> session_key;
		
	SessionManager();
	~SessionManager();
	
	bool Init(int keepAliveSeconds);
	bool Add(uint32_t key, const std::shared_ptr<Session>& session);
	void Remove(uint32_t key);
	std::shared_ptr<Session> Find(uint32_t key);
	size_t Size();

private :
	void OnTimerExpire(const boost::system::error_code& ec);
};

class Session : public std::enable_shared_from_this<Session>
{
public :
	struct InitFunctor
	{
		template <class T>
		T* operator() (T* session)
		{
			if(false == session->Init())
			{
				return nullptr;
			}	
			session->OnCreate();
			return session;
		}
	};

	struct ReleaseFunctor
	{
		template <class T>
		T* operator() (T* session)
		{
			if(nullptr == session)
			{
				return nullptr;
			}
			session->OnDestroy();
			return session;
		}
	};

	Session();
	virtual ~Session();

	uint32_t					session_key;
	std::string					session_token;
	boost::asio::ip::address*	remote_address;
	boost::asio::strand			strand;
	time_t						expire_time;
	std::shared_ptr<Link>		link;
	HandlerContainer			handler_container;

public :
	virtual void OnCreate() = 0;
	virtual void OnAccept() = 0;
	virtual void OnClose(int reason) = 0;
	virtual void OnDestroy() = 0;

	virtual bool Init();
	bool AsyncSend(const std::shared_ptr<Buffer>& buffer);
	bool AsyncSend(const char* data, int length);
	int SyncSend(const std::shared_ptr<Buffer>& buffer);
	int SyncSend(const char* data, int length);
		
	void AttachLink(const std::shared_ptr<Link>& link);
	static std::string GenerateSessionToken(uint32_t session_key);
	static void CreateWorkerThreadPool(uint32_t threadCount);
};


}} /* namespace Gamnet */

#endif /* SESSION_H_ */
