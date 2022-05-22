#ifndef GAMNET_NETWORK_SESSION_MANAGER_H_
#define GAMNET_NETWORK_SESSION_MANAGER_H_

#include "Session.h"
#include <mutex>

namespace Gamnet { namespace Network {
	class SessionManager
	{
	protected :
		std::mutex lock;
		std::map<uint32_t, std::shared_ptr<Session>> sessions;
	public :
		virtual void Add(const std::shared_ptr<Session>& session);
		virtual void Remove(const std::shared_ptr<Session>& session);
		virtual void OnReceive(const std::shared_ptr<Session>& session, const std::shared_ptr<Buffer>& buffer) = 0;

		std::shared_ptr<Session> Find(uint32_t sessionKey)
		{
			std::lock_guard<std::mutex> lo(lock);
			auto itr = sessions.find(sessionKey);
			if (sessions.end() == itr)
			{
				return nullptr;
			}
			return itr->second;
		}
	};
}}
#endif
