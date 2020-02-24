#ifndef GAMNET_NETWORK_ROUTER_MSGHANDLER_H_
#define GAMNET_NETWORK_ROUTER_MSGHANDLER_H_

#include "Session.h"
#include "../Handler.h"

namespace Gamnet { namespace Network { namespace Router {

struct RouterHandler : public Network::IHandler
{
	RouterHandler() {}
	virtual ~RouterHandler() {}

	void Recv_SetAddress_Req(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
	void Recv_SetAddress_Ans(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
	void Recv_SendMsg_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
	void Recv_HeartBeat_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
};

}}}
#endif /* ROUTER_MSGHANDLER_H_ */
