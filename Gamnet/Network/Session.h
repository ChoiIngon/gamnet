#ifndef GAMNET_NETWORK_SESSION_H_
#define GAMNET_NETWORK_SESSION_H_

#include <boost/asio.hpp>
#include <atomic>
#include <map>

#include "../Library/Buffer.h"
#include "../Library/Timer.h"
#include "../Log/Log.h"
#include "HandlerContainer.h"

namespace Gamnet { namespace Network {

class Link;
class LinkManager;
class Session;

class SessionManager
{
	Timer 	_timer;
	uint32_t	_keepalive_time;
	std::recursive_mutex _lock;
	std::map<uint32_t, std::shared_ptr<Session>> _sessions;
public :
	static std::atomic<uint32_t> session_key;

	SessionManager();
	~SessionManager();
	
	bool Init(int keepAliveSeconds);
	bool Add(uint32_t key, const std::shared_ptr<Session>& session);
	void Remove(uint32_t key);
	std::shared_ptr<Session> Find(uint32_t key);
	size_t Size();
};

class Session : public std::enable_shared_from_this<Session>
{
public :
	struct Init
	{
		template <class T>
		T* operator() (T* session)
		{
			session->session_key = ++Network::SessionManager::session_key;
			session->session_token = "";
			session->expire_time = 0;
			session->link = NULL;
			session->remote_address = NULL;
			session->handler_container.Init();
			return session;
		}
	};
	Session();
	virtual ~Session();

	uint32_t					session_key;
	std::string					session_token;
	boost::asio::ip::address*	remote_address;
	time_t						expire_time;
	std::shared_ptr<Link>		link;
	HandlerContainer			handler_container;
	LinkManager*				manager;

public :
	virtual void OnCreate() = 0;
	virtual void OnAccept() = 0;
	virtual void OnClose(int reason) = 0;
	virtual void OnDestroy() = 0;

	void AsyncSend(const std::shared_ptr<Buffer>& buffer);
	void AsyncSend(const char* data, int length);
	int SyncSend(const std::shared_ptr<Buffer>& buffer);
	int SyncSend(const char* data, int length);

	void OnRecv(const std::shared_ptr<Buffer>& buffer);
	static std::string GenerateSessionToken(uint32_t session_key);
};


}} /* namespace Gamnet */

#endif /* SESSION_H_ */
