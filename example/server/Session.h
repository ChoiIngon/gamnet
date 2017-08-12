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
	Session()
	{

	}
	virtual ~Session()
	{

	}

	virtual void OnAccept()
	{
		LOG(DEV, "session connected(session_key:", sessionKey_, ")");
	}

	virtual void OnClose(int reason)
	{
		LOG(DEV, "session closed(session_key:", sessionKey_, ")");
	}
	std::shared_ptr<UserData> user_data;
};

class TestSession : public Gamnet::Test::Session {
public :
	UserData user_data;
};

#endif /* SESSION_H_ */
