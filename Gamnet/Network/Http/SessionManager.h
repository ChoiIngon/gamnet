#ifndef GAMNET_NETWORK_HTTP_SESSION_MANAGER_H_
#define GAMNET_NETWORK_HTTP_SESSION_MANAGER_H_

#include "Session.h"
#include "../SessionManager.h"
#include "../Tcp/Acceptor.h"
#include "../../Library/Pool.h"

namespace Gamnet { namespace Network { namespace Http {
	class SessionManager : public Network::SessionManager
	{
		Tcp::Acceptor acceptor;
		Pool<Session, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> session_pool;
	public:
		virtual void OnAccept(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) override;
		virtual void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) override {}
		virtual void OnReceive(const std::shared_ptr<Session>& session, const std::shared_ptr<Buffer>& buffer) = 0;
		virtual void OnDestroy(uint32_t sessionKey) override {};
	};
}}}
#endif
