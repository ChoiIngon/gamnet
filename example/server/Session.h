/*
 * Session.h
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#ifndef SESSION_H_
#define SESSION_H_

#include "../../Gamnet/Gamnet.h"
#include "../idl/message.h"

class Session : public Gamnet::Network::Tcp::Session {
public:
	Session();
	virtual ~Session();
	virtual void OnCreate();
	virtual void OnAccept();
	virtual void OnClose(int reason);
	
	UserData user_data;
	uint32_t ack_seq;
};

class TestSession : public Gamnet::Test::Session {
public :
	UserData user_data;
};

#endif /* SESSION_H_ */
