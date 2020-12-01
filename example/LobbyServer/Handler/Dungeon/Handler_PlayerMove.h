#ifndef HANDLER_DUNGEON_PLAYER_MOVE_H_
#define HANDLER_DUNGEON_PLAYER_MOVE_H_

#include "../../UserSession.h"
#include <idl/MessageDungeon.h>

namespace Handler {	namespace Dungeon {

	class Handler_PlayerMove : public Gamnet::Network::IHandler
	{
	public:
		Handler_PlayerMove();
		virtual ~Handler_PlayerMove();

		void Recv_Ntf(const std::shared_ptr<UserSession>& session, const Message::Dungeon::MsgCliSvr_PlayerMove_Ntf& ntf);
	};
}}
#endif /* HANDLER_LOGIN_H_ */
