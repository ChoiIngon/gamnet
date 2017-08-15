/*
 * Manager_Session.h
 *
 *  Created on: 2017. 8. 13.
 *      Author: kukuta
 */

#ifndef MANAGER_SESSION_H_
#define MANAGER_SESSION_H_

#include <mutex>
#include <string>
#include <map>
#include "Session.h"

class Manager_Session {
	std::mutex lock;
	std::map<std::string, std::shared_ptr<UserData>> user_datas;
public:
	Manager_Session();
	virtual ~Manager_Session();
	void Init();
	bool Add(const std::string& key, const std::shared_ptr<UserData>& user_data);
	const std::shared_ptr<UserData> Find(const std::string& key);
	void Remove(const std::string& key);
};

#endif /* MANAGER_SESSION_H_ */
