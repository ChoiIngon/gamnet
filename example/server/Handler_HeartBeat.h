#ifndef HANDLER_HEARTBEAT_H_
#define HANDLER_HEARTBEAT_H_

#include "Session.h"

class Handler_HeartBeat : public Gamnet::Network::IHandler {
public:
	Handler_HeartBeat();
	virtual ~Handler_HeartBeat();

	void Recv_Ntf(const std::shared_ptr<Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

#endif /* HANDLER_HEARTBEAT_H_ */
