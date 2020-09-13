#ifndef HANDLER_ITEM_OPEN_PACKAGE_ITEM_H_
#define HANDLER_ITEM_OPEN_PACKAGE_ITEM_H_

#include "../../UserSession.h"
#include <idl/MessageItem.h>

namespace Handler {	namespace Item {

	class Handler_OpenPackage : public Gamnet::Network::IHandler
	{
	public:
		Handler_OpenPackage();
		virtual ~Handler_OpenPackage();

		void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_OpenPackage_Req& req);
	};

}}
#endif /* HANDLER_LOGIN_H_ */
