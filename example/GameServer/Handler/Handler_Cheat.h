#ifndef HANDLER_CHEAT_H_
#define HANDLER_CHEAT_H_

#include "../UserSession.h"
#include "../../idl/MessageUser.h"


namespace Handler {
class Handler_Cheat : public Gamnet::Network::IHandler
{
public:
	Handler_Cheat() = default;
	virtual ~Handler_Cheat() = default;

	void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::User::MsgCliSvr_Cheat_Req& req);
};
}

#endif /* HANDLER_LOGIN_H_ */
