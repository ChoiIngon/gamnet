#ifndef USER_LOGIN_HANDLER_DELETE_H_
#define USER_LOGIN_HANDLER_DELETE_H_

#include "../../UserSession.h"
#include "../../../idl/MessageUser.h"

namespace Handler { namespace User {

class Handler_Delete : public Gamnet::Network::IHandler
{
public:
	Handler_Delete();
	virtual ~Handler_Delete();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::User::MsgCliSvr_Delete_Req& req);
};
}}
#endif 
