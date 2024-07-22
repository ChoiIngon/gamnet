#ifndef HANDLER_ITEM_SELL_ITEM_H_
#define HANDLER_ITEM_SELL_ITEM_H_

#include "../../UserSession.h"
#include <idl/MessageItem.h>

namespace Handler {	namespace Item {

	class Handler_SellItem : public Gamnet::Network::IHandler
	{
	public:
		Handler_SellItem();
		virtual ~Handler_SellItem();

		void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Item::MsgCliSvr_SellItem_Req& req);
	};

}}
#endif /* HANDLER_LOGIN_H_ */
