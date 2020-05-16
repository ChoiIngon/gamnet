#ifndef GAMNET_NETWORK_SERVICE_H_
#define GAMNET_NETWORK_SERVICE_H_

#include "Session.h"

namespace Gamnet { namespace Network {
	class SessionManager {
	public :
		virtual std::shared_ptr<Session> OnAccept(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) = 0;
		virtual std::shared_ptr<Session> OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) = 0;
		virtual void OnReceive(const std::shared_ptr<Session>& session, const std::shared_ptr<Buffer>& buffer) = 0;
		virtual void OnDestroy(uint32_t sessionKey) = 0;
	};
}}
#endif
