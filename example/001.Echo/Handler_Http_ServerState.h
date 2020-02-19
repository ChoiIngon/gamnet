#ifndef HANDLER_HTTP_ServerState_H_
#define HANDLER_HTTP_ServerState_H_

#include "EchoSession.h"

class Handler_Http_ServerState : public Gamnet::Network::IHandler
{
public:
	Handler_Http_ServerState();
	virtual ~Handler_Http_ServerState();

	void Recv_Req(const std::shared_ptr<Gamnet::Network::Http::Session>& session, const Gamnet::Network::Http::Request& param);
};

#endif
