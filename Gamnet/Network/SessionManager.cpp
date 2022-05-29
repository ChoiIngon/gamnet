#include "SessionManager.h"

namespace Gamnet { namespace Network {

void SessionManager::Add(const std::shared_ptr<Session>& session)
{
	std::lock_guard<std::mutex> lo(lock);
	if (false == sessions.insert(std::make_pair(session->session_key, session)).second)
	{
		assert(!"duplicated session");
		throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "duplicated session");
	}
}

void SessionManager::Remove(const std::shared_ptr<Network::Session>& session)
{
	std::lock_guard<std::mutex> lo(lock);
#ifdef _DEBUG
    assert(sessions.end() != sessions.find(session->session_key));
#endif
	sessions.erase(session->session_key);
}

}}