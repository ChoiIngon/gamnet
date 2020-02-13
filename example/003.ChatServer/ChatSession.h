/*
 * Session.h
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#ifndef SESSION_H_
#define SESSION_H_

#include <Gamnet.h>
#include "ChatMessage.h"

class ChatSession : public Gamnet::Network::Tcp::Session {
public:
	ChatSession();
	virtual ~ChatSession();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	
	ChatUserData user_data;
	std::shared_ptr<Gamnet::Network::Tcp::CastGroup> chat_channel;
};

class Manager_Session {
private:
	Gamnet::Timer timer;
	std::mutex lock;

	std::map<std::string, std::shared_ptr<ChatSession>> sessions;
public:
	Manager_Session();
	virtual ~Manager_Session();
	void Init();
	const std::shared_ptr<ChatSession> Add(const std::string& user_id, const std::shared_ptr<ChatSession>& session);
	void Remove(const std::string& key);
};

class TestSession : public Gamnet::Test::Session {
public :
	int64_t channel_seq;
	int64_t chat_seq;
	ChatUserData user_data;
	std::set<std::string> user_ids;
	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

#endif /* SESSION_H_ */
