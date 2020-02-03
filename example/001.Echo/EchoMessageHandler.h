#ifndef ECHO_MESSAGE_HANDLER_H_
#define ECHO_MESSAGE_HANDLER_H_

#include "EchoSession.h"

class EchoMessageHandler : public Gamnet::Network::IHandler 
{
public:
	EchoMessageHandler();
	virtual ~EchoMessageHandler();

	void Recv_Req(const std::shared_ptr<EchoSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

#endif /* HANDLER_LOGIN_H_ */
