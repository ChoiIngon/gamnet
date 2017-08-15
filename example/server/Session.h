/*
 * Session.h
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#ifndef SESSION_H_
#define SESSION_H_

#include "../../Gamnet/Gamnet.h"
#include "message.h"

class Session : public Gamnet::Network::Session {
public:
	Session();
	virtual ~Session();
	virtual void OnAccept();
	virtual void OnClose(int reason);

	int msg_seq;
	std::shared_ptr<UserData> user_data;
};

class TestSession : public Gamnet::Test::Session {
public :
	UserData user_data;
};

#endif /* SESSION_H_ */
