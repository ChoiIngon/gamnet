#ifndef GAMNET_NETWORK_SERVICE_H_
#define GAMNET_NETWORK_SERVICE_H_

#include "Session.h"
#include "../Library/Debugs.h"

namespace Gamnet { namespace Network {
	class SessionManager {
	public :
		virtual void OnAccept(std::shared_ptr<boost::asio::ip::tcp::socket> socket) = 0;
	};
}}
#endif
