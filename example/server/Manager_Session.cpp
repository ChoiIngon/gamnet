/*
 * Manager_Session.cpp
 *
 *  Created on: 2017. 8. 13.
 *      Author: kukuta
 */

#include "Manager_Session.h"

Manager_Session::Manager_Session() {
}

Manager_Session::~Manager_Session() {
}

void Manager_Session::Init()
{
}

GAMNET_BIND_INIT_HANDLER(Manager_Session, Init);

const std::shared_ptr<Session> Manager_Session::Add(const std::string& user_id, const std::shared_ptr<Session>& session)
{
	std::lock_guard<std::mutex> lo(lock);
	std::shared_ptr<Session> old_value = NULL;
	auto itr = sessions.find(user_id);
	if(sessions.end() != itr)
	{
		old_value = itr->second;
	}
	sessions[user_id] = session;
	return old_value;
}

void Manager_Session::Remove(const std::string& user_id)
{
	std::lock_guard<std::mutex> lo(lock);
	sessions.erase(user_id);
}
