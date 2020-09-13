#ifndef HANDLER_DUNGEON_CREATE_H_
#define HANDLER_DUNGEON_CREATE_H_

#include "../../UserSession.h"
#include <idl/MessageDungeon.h>

namespace Handler {	namespace Dungeon {

	class Handler_CreateDungeon : public Gamnet::Network::IHandler
	{
	public:
		Handler_CreateDungeon();
		virtual ~Handler_CreateDungeon();

		void Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Dungeon::MsgCliSvr_CreateDungeon_Req& req);
	};
}}
#endif /* HANDLER_LOGIN_H_ */
