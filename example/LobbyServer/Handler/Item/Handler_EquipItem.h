#ifndef HANDLER_ITEM_EQUIP_ITEM_H_
#define HANDLER_ITEM_EQUIP_ITEM_H_

#include "../../UserSession.h"
#include <idl/MessageItem.h>

namespace Handler {	namespace Item {

	class Handler_EquipItem : public Gamnet::Network::IHandler
	{
	public:
		Handler_EquipItem();
		virtual ~Handler_EquipItem();

		void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_EquipItem_Req& req);
	};

}}
#endif /* HANDLER_LOGIN_H_ */
