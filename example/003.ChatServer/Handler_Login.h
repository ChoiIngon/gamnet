/*
 * Handler_Login.h
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#ifndef HANDLER_LOGIN_H_
#define HANDLER_LOGIN_H_

#include "ChatSession.h"

class Handler_Login : public Gamnet::Network::IHandler {
public:
	Handler_Login();
	virtual ~Handler_Login();

	void Recv_Req(const std::shared_ptr<ChatSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

#endif /* HANDLER_LOGIN_H_ */
