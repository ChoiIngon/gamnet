#ifndef USER_LOGIN_HANDLER_H_
#define USER_LOGIN_HANDLER_H_

#include "UserSession.h"

class UserLoginHandler : public Gamnet::Network::IHandler
{
public:
	UserLoginHandler();
	virtual ~UserLoginHandler();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

#endif /* HANDLER_LOGIN_H_ */
