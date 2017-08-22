/*
 * Handler_Reconnect.h
 *
 *  Created on: 2017. 8. 18.
 *      Author: kukuta
 */

#ifndef HANDLER_RECONNECT_H_
#define HANDLER_RECONNECT_H_

#include "Session.h"

class Handler_Reconnect : public Gamnet::Network::IHandler {
public:
	Handler_Reconnect();
	virtual ~Handler_Reconnect();

	void Recv_Req(const std::shared_ptr<Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

#endif /* HANDLER_RECONNECT_H_ */
