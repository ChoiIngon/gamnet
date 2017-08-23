#ifndef GAMNET_NETWORK_SESSION_H_
#define GAMNET_NETWORK_SESSION_H_

#include <boost/asio.hpp>
#include <map>
#include "HandlerContainer.h"
#include "../Library/Buffer.h"
#include "../Library/Timer.h"
#include "../Log/Log.h"

namespace Gamnet { namespace Network {

class Link;
class LinkManager;
class Session : public std::enable_shared_from_this<Session>
{
public :
	struct Init
	{
		template <class T>
		T* operator() (T* session)
		{
			session->session_key = "";
			session->expire_time = 0;
			session->link = std::shared_ptr<Link>(NULL);;
			session->remote_address = NULL;
			return session;
		}
	};
	Session();
	virtual ~Session();

	std::string session_key;
	boost::asio::ip::address* remote_address;
	uint32_t expire_time;
	std::weak_ptr<Link> link;
	LinkManager* manager;
	HandlerContainer handler_container;

	virtual void OnAccept() = 0;
	virtual void OnClose(int reason) = 0;

	void AsyncSend(const std::shared_ptr<Buffer>& buffer);
	void AsyncSend(const char* data, size_t length);
	int SyncSend(const std::shared_ptr<Buffer>& buffer);
	int SyncSend(const char* data, size_t length);
};

class SessionManager
{
	Timer 	_timer;
	uint32_t	_keepalive_time;
	std::recursive_mutex _lock;
	std::map<std::string, std::shared_ptr<Session>> _sessions;
public :
	SessionManager();
	~SessionManager();

	bool Init(int keepAliveSeconds);
	bool Add(const std::string& key, const std::shared_ptr<Session>& session);
	void Remove(const std::string& key);
	std::shared_ptr<Session> Find(const std::string& key);
	size_t Size();
};

}} /* namespace Gamnet */

#endif /* SESSION_H_ */
