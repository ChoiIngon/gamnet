/*
 * Session.h
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#ifndef SESSION_H_
#define SESSION_H_

#include <Gamnet.h>
#include "../idl/Message.h"

class UserSession : public Gamnet::Network::Tcp::Session 
{
public:
	UserSession();
	virtual ~UserSession();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	
	GUserData user_data;
	std::shared_ptr<Gamnet::Network::Tcp::CastGroup> chat_channel;
};

class Manager_Session 
{
private:
	Gamnet::Timer timer;
	std::mutex lock;

	std::map<std::string, std::shared_ptr<UserSession>> sessions;
public:
	Manager_Session();
	virtual ~Manager_Session();
	void Init();
	const std::shared_ptr<UserSession> Add(const std::string& user_id, const std::shared_ptr<UserSession>& session);
	void Remove(const std::string& key);
};

class TestSession : public Gamnet::Test::Session {
public :
	int64_t channel_seq;
	int64_t chat_seq;
	GUserData user_data;
	std::set<std::string> user_ids;
	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

#endif /* SESSION_H_ */
