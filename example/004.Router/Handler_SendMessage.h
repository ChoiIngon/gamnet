#ifndef HANDLER_ROUTER_SENDMESSAGE_H_
#define HANDLER_ROUTER_SENDMESSAGE_H_

#include "UserSession.h"
#include <Gamnet/Network/Tcp/Handler.h>

class Handler_SendMessage : public Gamnet::Network::Tcp::Handler {
public:
	Handler_SendMessage();
	virtual ~Handler_SendMessage();

	void Recv_CliSvr_Req(const std::shared_ptr<UserSession>& session, const MsgCliSvr_SendMessage_Req& req);
	void Recv_SvrSvr_Req(const Gamnet::Network::Router::Address& address, const MsgSvrSvr_SendMessage_Req& req);
	void Recv_SvrSvr_Ans(const MsgSvrSvr_SendMessage_Ans& ansSvrSvr);

	void Recv_CliSvr_Ntf(const std::shared_ptr<UserSession>& session, const MsgCliSvr_SendMessage_Ntf& ntfCliSvr);
	void Recv_SvrSvr_Ntf(const Gamnet::Network::Router::Address& address, const MsgSvrSvr_SendMessage_Ntf& ntfSvrSvr);

private :
	std::shared_ptr<UserSession> session;
};

#endif /* HANDLER_LOGIN_H_ */
