#ifndef HANDLER_HTTP_ServerState_H_
#define HANDLER_HTTP_ServerState_H_

#include "Session.h"

class Handler_Http_ServerState : public Gamnet::Network::IHandler
{
public:
	Handler_Http_ServerState();
	~Handler_Http_ServerState();

	void Recv_Req(const std::shared_ptr<Gamnet::Network::Http::Session>& session, const std::map<std::string, std::string>& param);
};

#endif