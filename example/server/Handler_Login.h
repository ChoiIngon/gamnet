/*
 * Handler_Login.h
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#ifndef HANDLER_LOGIN_H_
#define HANDLER_LOGIN_H_

#include "Session.h"

class Handler_Login : public Gamnet::Network::IHandler {
public:
	Handler_Login();
	virtual ~Handler_Login();

	void Recv_Req(std::shared_ptr<Session> session, std::shared_ptr<Gamnet::Network::Packet> packet);
};

#endif /* HANDLER_LOGIN_H_ */
