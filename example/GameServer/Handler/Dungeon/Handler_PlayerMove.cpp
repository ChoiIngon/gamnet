#include "Handler_PlayerMove.h"
#include "../../Component/UserData.h"
#include "../../Component/Dungeon/Dungeon.h"
#include "../../Component/Dungeon/AStarPathFinder.h"
#include "../../Component/Dungeon/Unit.h"
#include "../../Component/Dungeon/Unit/Monster.h"

namespace Handler { namespace Dungeon {

Handler_PlayerMove::Handler_PlayerMove()
{
}

Handler_PlayerMove::~Handler_PlayerMove()
{
}

void Handler_PlayerMove::Recv_Ntf(const std::shared_ptr<UserSession>& session, const Message::Dungeon::MsgCliSvr_PlayerMove_Ntf& ntf)
{
	try {
		LOG(DEV, "Message::Dungeon::MsgCliSvr_PlayerMove_Ntf()");
		std::shared_ptr<Component::Dungeon::Data> dungeon = session->GetComponent<Component::Dungeon::Data>();
		if (nullptr == dungeon)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		std::shared_ptr<Component::Unit::Data> player = session->GetComponent<Component::Unit::Data>();
		if(nullptr == player)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		player->path_finder = std::make_shared<AStarPathFinder>(*dungeon, player->position, Vector2Int(ntf.destination.x, ntf.destination.y));
		player->timer->Cancel();
		player->timer->ExpireRepeat(100, [dungeon, player, session](){
			std::shared_ptr<AStarPathFinder> pathFinder = player->path_finder;
			const Vector2Int position = pathFinder->path.front();
			pathFinder->path.pop_front();
			player->SetPosition(position);

			Message::Dungeon::MsgSvrCli_UnitPosition_Ntf ntf;
			ntf.unit_seq = player->seq;
			ntf.position.x = position.x;
			ntf.position.y = position.y;
			player->dungeon->SendMsg(ntf);

			if(0 == pathFinder->path.size())
			{
				player->timer->Cancel();
			}
		});
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::Dungeon::MsgCliSvr_PlayerMove_Ntf,
	Handler_PlayerMove, Recv_Ntf,
	HandlerStatic
);

}}