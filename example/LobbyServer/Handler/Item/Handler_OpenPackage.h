#ifndef HANDLER_ITEM_OPEN_PACKAGE_ITEM_H_
#define HANDLER_ITEM_OPEN_PACKAGE_ITEM_H_

#include "../../UserSession.h"

namespace Handler {	namespace Item {

	class Handler_OpenPackage : public Gamnet::Network::IHandler
	{
	public:
		Handler_OpenPackage();
		virtual ~Handler_OpenPackage();

		void Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);
	};

}}
#endif /* HANDLER_LOGIN_H_ */
