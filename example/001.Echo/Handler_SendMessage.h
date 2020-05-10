#ifndef HANDLER_SENDMESSAGE_H_
#define HANDLER_SENDMESSAGE_H_

#include "UserSession.h"

class Handler_SendMessage : public Gamnet::Network::IHandler {
public:
	Handler_SendMessage();
	virtual ~Handler_SendMessage();

	void Recv_Ntf(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
//	void Recv_SvrSvr_Req(const Gamnet::Network::Router::Address& address, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
//	void Recv_SvrSvr_Ans(const Gamnet::Network::Router::Address& address, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

//void StartRouterMessageTimer();
//void OnRouterConnect(const Gamnet::Network::Router::Address& address);
//void OnRouterClose(const Gamnet::Network::Router::Address& address);
#endif /* HANDLER_LOGIN_H_ */
