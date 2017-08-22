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

class Session : public Gamnet::Network::Tcp::Session {
public:
	Session();
	virtual ~Session();
	virtual void OnAccept();
	virtual void OnClose(int reason);

	uint32_t ack_seq;
	std::shared_ptr<UserData> user_data;
};

class TestSession : public Gamnet::Test::Session {
public :
	UserData user_data;
};


#ifdef _WIN32
#define ERROR(error) error, "ERR [", __FILE__, ":", __FUNCTION__, "@" , __LINE__, "] ", #error, " "
#else
#define ERROR(error) error, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] ", #error, " "
#endif

#endif /* SESSION_H_ */
