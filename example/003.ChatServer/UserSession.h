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
#include <Library/Component.h>

const int CHAT_QUORUM = 4;

class UserSession : public Gamnet::Network::Tcp::Session {
public:
	UserSession();
	virtual ~UserSession();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	
	std::shared_ptr<Gamnet::Component> component;
	std::shared_ptr<Gamnet::Network::Tcp::CastGroup> chat_channel;
};

/*
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
*/

class TestSession : public Gamnet::Test::Session {
public :
	const int CHAT_QUORUM = 4;
	uint32_t channel_seq;
	int32_t recv_count;
	std::set<std::string> user_ids;

	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
};

#endif /* SESSION_H_ */
