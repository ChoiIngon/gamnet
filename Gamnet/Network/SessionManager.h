#ifndef GAMNET_NETWORK_SERVICE_H_
#define GAMNET_NETWORK_SERVICE_H_

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

		template <class SESSION_T>
		std::shared_ptr<SESSION_T> Find(uint32_t sessionKey)
		{
			std::lock_guard<std::mutex> lo(lock);
			auto itr = sessions.find(sessionKey);
			if (sessions.end() == itr)
			{
				return nullptr;
			}
			return std::static_pointer_cast<SESSION_T>(itr->second);
		}
	};
}}
#endif
