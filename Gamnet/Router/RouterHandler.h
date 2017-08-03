/*
 * Router_MsgHandler.h
 *
 *  Created on: Jun 24, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_ROUTER_MSGHANDLER_H_
#define GAMNET_ROUTER_MSGHANDLER_H_

#include "Session.h"
#include "../Network/Handler.h"

namespace Gamnet { namespace Router {

struct RouterHandler : public Network::IHandler
{
	RouterHandler() {}
	virtual ~RouterHandler() {}

	void Recv_SetAddress_Req(std::shared_ptr<Session> session, std::shared_ptr<Network::Packet> packet);
	void Recv_SetAddress_Ans(std::shared_ptr<Session> session, std::shared_ptr<Network::Packet> packet);
	void Recv_SetAddress_Ntf(std::shared_ptr<Session> session, std::shared_ptr<Network::Packet> packet);
	void Recv_SendMsg_Ntf(std::shared_ptr<Session> session, std::shared_ptr<Network::Packet> packet);
};

}}
#endif /* ROUTER_MSGHANDLER_H_ */
