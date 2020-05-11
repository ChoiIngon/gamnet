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
			return session;
		}
	};

	struct ReleaseFunctor
	{
		template <class T>
		T* operator() (T* session)
		{
			if(nullptr != session)
			{
				session->Clear();
			}
			return session;
		}
	};

	Session();
	virtual ~Session();

	boost::asio::strand			strand;
	uint32_t					session_key;
	std::string					session_token;
	std::shared_ptr<Link>		link;
	HandlerContainer			handler_container;
	std::shared_ptr<boost::asio::ip::tcp::socket> socket;
public :
	virtual void OnCreate() = 0;
	virtual void OnAccept() = 0;
	virtual void OnClose(int reason) = 0;
	virtual void OnDestroy() = 0;

	virtual bool Init();
	virtual void Clear();
	void AsyncSend(const std::shared_ptr<Buffer> buffer);
	void AsyncSend(const char* data, size_t length);
	int SyncSend(const std::shared_ptr<Buffer> buffer);
	int SyncSend(const char* data, int length);
	
	const boost::asio::ip::address& GetRemoteAddress() const;
	static std::string GenerateSessionToken(uint32_t session_key);

	void OnAcceptHandler();
};


}} /* namespace Gamnet */

#endif /* SESSION_H_ */
