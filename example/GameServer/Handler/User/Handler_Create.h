#ifndef HANDLER_USER_CREATE_H_
#define HANDLER_USER_CREATE_H_

#include "../../UserSession.h"
#include "../../../idl/MessageUser.h"


namespace Handler {	namespace User {
	class Handler_Create : public Gamnet::Network::IHandler
	{
	public:
		Handler_Create() = default;
		virtual ~Handler_Create() = default;

		void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::User::MsgCliSvr_Create_Req& req);
	};
}}
#endif /* HANDLER_LOGIN_H_ */
