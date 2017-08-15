/*
 * Manager_Session.cpp
 *
 *  Created on: 2017. 8. 13.
 *      Author: kukuta
 */

#include "Manager_Session.h"

Manager_Session::Manager_Session() {
	// TODO Auto-generated constructor stub

}

Manager_Session::~Manager_Session() {
	// TODO Auto-generated destructor stub
}

void Manager_Session::Init()
{
}

GAMNET_BIND_INIT_HANDLER(Manager_Session, Init);

bool Manager_Session::Add(const std::string& key, const std::shared_ptr<UserData>& user_data)
{
	std::lock_guard<std::mutex> lo(lock);
	return user_datas.insert(std::make_pair(key, user_data)).second;
}

const std::shared_ptr<UserData> Manager_Session::Find(const std::string& key)
{
	std::lock_guard<std::mutex> lo(lock);
	auto itr = user_datas.find(key);
	if(user_datas.end() == itr)
	{
		return NULL;
	}
	return itr->second;
}

void Manager_Session::Remove(const std::string& key)
{
	std::lock_guard<std::mutex> lo(lock);
	user_datas.erase(key);
}
