#ifndef HANDLER_LOBBY_JOIN_H_
#define HANDLER_LOBBY_JOIN_H_

#include "../../UserSession.h"

namespace Handler {	namespace Lobby {

class Handler_JoinLobby : public Gamnet::Network::IHandler
{
public:
	Handler_JoinLobby();
	virtual ~Handler_JoinLobby();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);

private :
	void LoadUserMail(const std::shared_ptr<UserSession>& session);
};

}}
#endif /* HANDLER_LOGIN_H_ */
