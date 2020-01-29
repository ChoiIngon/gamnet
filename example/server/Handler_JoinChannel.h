#ifndef HANDLER_JOINCHANNEL_H_
#define HANDLER_JOINCHANNEL_H_

#include "Session.h"

class Handler_JoinChannel : public Gamnet::Network::IHandler {
public:
	Handler_JoinChannel();
	virtual ~Handler_JoinChannel();

	void Recv_Req(const std::shared_ptr<Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

#endif /* HANDLER_LOGIN_H_ */
