#ifndef USER_LOGIN_HANDLER_H_
#define USER_LOGIN_HANDLER_H_

#include "../../UserSession.h"
#include <Gamnet/Library/MetaData.h>

namespace Handler { namespace User {

class Handler_Login : public Gamnet::Network::IHandler
{
public:
	Handler_Login();
	virtual ~Handler_Login();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);

private :
	void ReadUserData(const std::shared_ptr<UserSession>& session);
	void ReadUserCounter(const std::shared_ptr<UserSession>& session);
	void ReadUserItem(const std::shared_ptr<UserSession>& session);
};
}}
#endif /* HANDLER_LOGIN_H_ */
