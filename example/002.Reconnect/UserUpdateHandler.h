#ifndef USER_UPDATE_HANDLER_H_
#define USER_UPDATE_HANDLER_H_

#include "UserSession.h"

class UserUpdateHandler : public Gamnet::Network::IHandler
{
public:
	UserUpdateHandler();
	virtual ~UserUpdateHandler();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};

#endif /* HANDLER_LOGIN_H_ */
