#ifndef USER_LOGIN_HANDLER_DELETE_H_
#define USER_LOGIN_HANDLER_DELETE_H_

#include "../../UserSession.h"

namespace Handler { namespace User {

class Handler_Delete : public Gamnet::Network::IHandler
{
public:
	Handler_Delete();
	virtual ~Handler_Delete();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
};
}}
#endif 
