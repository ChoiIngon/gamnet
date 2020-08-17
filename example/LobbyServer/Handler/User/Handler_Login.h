#ifndef USER_LOGIN_HANDLER_H_
#define USER_LOGIN_HANDLER_H_

#include "../../UserSession.h"
#include "../../../idl/MessageUser.h"
#include <Gamnet/Library/MetaData.h>

namespace Handler { namespace User {
	
class Handler_Login : public Gamnet::Network::IHandler
{
public:
	Handler_Login();
	virtual ~Handler_Login();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::User::MsgCliSvr_Login_Req& req);

private :
	void ReadAccountData(const std::shared_ptr<UserSession>& session, const std::string& accountID, Message::AccountType accountType);
	void ReadUserData(const std::shared_ptr<UserSession>& session);
	void ReadUserCounter(const std::shared_ptr<UserSession>& session);
	void ReadUserItem(const std::shared_ptr<UserSession>& session);
};
}}
#endif /* HANDLER_LOGIN_H_ */
