#include "SessionManager.h"

namespace Gamnet { namespace Network {

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
}

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
	sessions.erase(session->session_key);
}

}}