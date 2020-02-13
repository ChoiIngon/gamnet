#ifndef HANDLER_LEAVECHANNEL_H_
#define HANDLER_LEAVECHANNEL_H_

#include "ChatSession.h"

class Handler_LeaveChannel : public Gamnet::Network::IHandler {
public:
	Handler_LeaveChannel();
	virtual ~Handler_LeaveChannel();

	void Recv_Req(const std::shared_ptr<ChatSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

#endif /* HANDLER_LOGIN_H_ */
