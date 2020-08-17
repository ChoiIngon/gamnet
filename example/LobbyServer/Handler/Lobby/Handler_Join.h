#ifndef HANDLER_LOBBY_JOIN_H_
#define HANDLER_LOBBY_JOIN_H_

#include "../../UserSession.h"
#include "../../../idl/MessageLobby.h"

namespace Handler {	namespace Lobby {

class Handler_Join : public Gamnet::Network::IHandler
{
public:
	Handler_Join();
	virtual ~Handler_Join();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Lobby::MsgCliSvr_Join_Req& req);

private :
	void LoadUserEvent(const std::shared_ptr<UserSession>& session);
	void LoadUserMail(const std::shared_ptr<UserSession>& session);
};

}}
#endif /* HANDLER_LOGIN_H_ */
