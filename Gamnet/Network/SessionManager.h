#ifndef GAMNET_NETWORK_SERVICE_H_
#define GAMNET_NETWORK_SERVICE_H_

#include "Session.h"

namespace Gamnet { namespace Network {
	class SessionManager 
	{
	public :
		virtual void Add(const std::shared_ptr<Session>& session) = 0;
		virtual void Remove(const std::shared_ptr<Session>& session) = 0;
		virtual void OnReceive(const std::shared_ptr<Session>& session, const std::shared_ptr<Buffer>& buffer) = 0;
	};
}}
#endif
