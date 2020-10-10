#ifndef USER_LOGIN_HANDLER_H_
#define USER_LOGIN_HANDLER_H_

#include "UserSession.h"

class Handler_Login : public Gamnet::Network::IHandler
{
public:
	Handler_Login();
	virtual ~Handler_Login();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const MsgCliSvr_Login_Req& req);
};

#endif /* HANDLER_LOGIN_H_ */
