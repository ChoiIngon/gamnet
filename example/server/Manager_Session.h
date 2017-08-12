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
	std::map<std::string, std::shared_ptr<Session>> sessions;
public:
	Manager_Session();
	virtual ~Manager_Session();

	bool Add(const std::string& key, const std::shared_ptr<UserData>& user_data);
};

#endif /* MANAGER_SESSION_H_ */
