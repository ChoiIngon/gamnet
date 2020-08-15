#ifndef HANDLER_USER_CREATE_H_
#define HANDLER_USER_CREATE_H_

#include "../../UserSession.h"
#include <Gamnet/Library/MetaData.h>

namespace Handler {	namespace User {
	class Handler_Create : public Gamnet::Network::IHandler
	{
	public:
		Handler_Create();
		virtual ~Handler_Create();

		void Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
	};
}}
#endif /* HANDLER_LOGIN_H_ */
