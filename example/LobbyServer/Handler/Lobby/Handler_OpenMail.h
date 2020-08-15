#ifndef HANDLER_LOBBY_OPON_MAIL_H_
#define HANDLER_LOBBY_OPON_MAIL_H_

#include "../../UserSession.h"

namespace Handler {	namespace Lobby {

	class Handler_OpenMail : public Gamnet::Network::IHandler
	{
	public:
		Handler_OpenMail();
		virtual ~Handler_OpenMail();

		void Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
	};

}}
#endif /* HANDLER_LOGIN_H_ */
