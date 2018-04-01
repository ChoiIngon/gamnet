/*
 * Session.h
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#ifndef SESSION_H_
#define SESSION_H_

#include <Gamnet.h>
#include "../idl/message.h"

class Session : public Gamnet::Network::Tcp::Session {
public:
	Session();
	virtual ~Session();
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	
	UserData user_data;
	uint32_t ack_seq;

	//void Reconnect();
};

class TestSession : public Gamnet::Test::Session {
	std::string host;
public :
	UserData user_data;
	virtual void OnCreate() override;
	virtual void OnConnect() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;

	void Reconnect();
};

#endif /* SESSION_H_ */
