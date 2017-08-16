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
	timer.SetTimer(60, [&]() {
		std::lock_guard<std::mutex> lo(lock);
		time_t curr = time(NULL);
		for(auto itr=user_datas.begin(); itr!=user_datas.end();)
		{
			const std::shared_ptr<UserData>& user_data = itr->second;
			if(user_data->kickout_time < curr)
			{
				LOG(DEV, "timeout session cache(session_key:", user_data->session_key, ")");
				user_datas.erase(itr++);
			}
			else
			{
				++itr;
			}
		}
		timer.Resume();
	});
}

GAMNET_BIND_INIT_HANDLER(Manager_Session, Init);

const std::shared_ptr<UserData> Manager_Session::Add(const std::string& user_id, const std::shared_ptr<UserData>& user_data)
{
	std::lock_guard<std::mutex> lo(lock);
	std::shared_ptr<UserData> old_value = std::shared_ptr<UserData>(NULL);
	auto itr = user_datas.find(user_id);
	if(user_datas.end() != itr)
	{
		old_value = itr->second;
	}
	user_datas[user_id] = user_data;
	return old_value;
}

void Manager_Session::Remove(const std::string& user_id)
{
	std::lock_guard<std::mutex> lo(lock);
	user_datas.erase(user_id);
}
