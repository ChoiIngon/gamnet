#ifndef GAMNET_NETWORK_SERVICE_H_
#define GAMNET_NETWORK_SERVICE_H_

#include "Session.h"
#include "../Library/Debugs.h"

namespace Gamnet { namespace Network {
	class SessionManager {
	public :
		boost::asio::io_service io_service;
		virtual void Create(const std::shared_ptr<boost::asio::ip::tcp::socket> socket) = 0;
		virtual void Destory(uint32_t sessionKey) = 0;
	};
}}
#endif
