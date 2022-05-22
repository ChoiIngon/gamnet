#ifndef HANDLER_HTTP_SERVERSTATE_H_
#define HANDLER_HTTP_SERVERSTATE_H_

#include <Gamnet.h>

namespace Handler { namespace Http {
class Handler_ServerState : public Gamnet::Network::IHandler {
public:
	Handler_ServerState();
	virtual ~Handler_ServerState();

	void Recv_ServerState_Req(const std::shared_ptr<Gamnet::Network::Http::Session>& session, const Gamnet::Network::Http::Request& request);
};

}}
#endif /* HANDLER_HTTP_SERVERSTATE_H_ */
