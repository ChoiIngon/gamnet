/*
 * Handler_Move.h
 *
 *  Created on: 2017. 8. 15.
 *      Author: kukuta
 */

#ifndef HANDLER_MOVE_H_
#define HANDLER_MOVE_H_

#include "Session.h"

class Handler_Move : public Gamnet::Network::IHandler {
public:
	Handler_Move();
	virtual ~Handler_Move();

	void Recv_Ntf(std::shared_ptr<Session> session, std::shared_ptr<Gamnet::Network::Packet> packet);
};

#endif /* HANDLER_MOVE_H_ */
