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
private :
	Gamnet::Timer timer;
	std::mutex lock;

	std::map<std::string, std::shared_ptr<Session>> sessions;
public:
	Manager_Session();
	virtual ~Manager_Session();
	void Init();
	const std::shared_ptr<Session> Add(const std::string& user_id, const std::shared_ptr<Session>& session);
	void Remove(const std::string& key);
};

#endif /* MANAGER_SESSION_H_ */
