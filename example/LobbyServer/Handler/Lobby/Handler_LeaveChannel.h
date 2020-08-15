#ifndef HANDLER_LOBBY_JOIN_H_
#define HANDLER_LOBBY_JOIN_H_

#include "../../UserSession.h"

namespace Handler {	namespace Lobby {

class Handler_LeaveChannel : public Gamnet::Network::IHandler
{
public:
	Handler_LeaveChannel();
	virtual ~Handler_LeaveChannel();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

}}
#endif /* HANDLER_LOGIN_H_ */
