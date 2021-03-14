#ifndef GAMNET_NETWORK_ROUTER_MSGHANDLER_H_
#define GAMNET_NETWORK_ROUTER_MSGHANDLER_H_

#include "Session.h"
#include "MsgRouter.h"
#include "../Handler.h"

namespace Gamnet { namespace Network { namespace Router {

struct RouterHandler : public Network::IHandler
{
	RouterHandler() {}
	virtual ~RouterHandler() {}

	void Recv_Connect_Req(const std::shared_ptr<Session>& session, const MsgRouter_Connect_Req& req);
	void Recv_Connect_Ans(const std::shared_ptr<Session>& session, const MsgRouter_Connect_Ans& ans);
	void Recv_RegisterAddress_Req(const std::shared_ptr<Session>& session, const MsgRouter_RegisterAddress_Req& req);
	void Recv_RegisterAddress_Ans(const std::shared_ptr<Session>& session, const MsgRouter_RegisterAddress_Ans& ans);
	void Recv_SendMsg_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_SendMsg_Ntf& ntf);
	void Recv_HeartBeat_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_HeartBeat_Ntf& ntf);
	void Recv_RegisterAddress_Ntf(const std::shared_ptr<Session>& session, const MsgRouter_RegisterAddress_Ntf& ntf);
};

}}}
#endif /* ROUTER_MSGHANDLER_H_ */
