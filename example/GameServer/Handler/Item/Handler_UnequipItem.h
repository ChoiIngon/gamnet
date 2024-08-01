#ifndef HANDLER_ITEM_UNEQUIP_ITEM_H_
#define HANDLER_ITEM_UNEQUIP_ITEM_H_

#include "../../UserSession.h"
#include <idl/MessageItem.h>

namespace Handler {	namespace Item {

	class Handler_UnequipItem : public Gamnet::Network::IHandler
	{
	public:
		Handler_UnequipItem() = default;
		virtual ~Handler_UnequipItem() = default;

		void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_UnequipItem_Req& req);
	};

}}
#endif /* HANDLER_LOGIN_H_ */
