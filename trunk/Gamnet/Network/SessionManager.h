#ifndef GAMNET_NETWORK_SESSION_MANAGER_H_
#define GAMNET_NETWORK_SESSION_MANAGER_H_

#include <map>
#include <mutex>
#include "Session.h"
#include "../Library/Timer.h"

namespace Gamnet { namespace Network {

class SessionManager
{
	Timer timer_;
	int keepAliveSec_;
	std::recursive_mutex lock_;
	std::map<uint64_t, std::shared_ptr<Session>> mapSession_;
public :
	SessionManager();
	~SessionManager();

	bool Init(int nKeepAliveSec);
	bool AddSession(uint64_t key, std::shared_ptr<Session> pSession);
	void DelSession(uint64_t key, std::shared_ptr<Session> pSession);
	std::shared_ptr<Session> FindSession(uint64_t key);
	size_t Size();
};

}}

#endif
