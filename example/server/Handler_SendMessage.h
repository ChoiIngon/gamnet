#ifndef HANDLER_SENDMESSAGE_H_
#define HANDLER_SENDMESSAGE_H_

#include "Session.h"

class Handler_SendMessage : public Gamnet::Network::IHandler {
public:
	Handler_SendMessage();
	virtual ~Handler_SendMessage();

	void Recv_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

#endif /* HANDLER_LOGIN_H_ */
