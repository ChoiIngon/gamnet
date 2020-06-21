#ifndef HANDLER_ROUTER_SENDMESSAGE_H_
#define HANDLER_ROUTER_SENDMESSAGE_H_

#include "UserSession.h"

class Handler_SendMessage : public Gamnet::Network::IHandler {
	std::shared_ptr<UserSession> session;
public:
	Handler_SendMessage();
	virtual ~Handler_SendMessage();

	void Recv_CliSvr_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
	void Recv_SvrSvr_Req(const Gamnet::Network::Router::Address& address, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
	void Recv_SvrSvr_Ans(const Gamnet::Network::Router::Address& address, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);

	void Recv_CliSvr_Ntf(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
	void Recv_SvrSvr_Ntf(const Gamnet::Network::Router::Address& address, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

void StartRouterMessageTimer();
#endif /* HANDLER_LOGIN_H_ */
