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

void Handler_PlayerMove::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Dungeon::MsgCliSvr_PlayerMove_Req& req)
{
	Message::Dungeon::MsgSvrCli_PlayerMove_Ans ans;
	ans.error_code = Message::ErrorCode::Success;
	
	try {
		LOG(DEV, "Message::Dungeon::MsgCliSvr_PlayerMove_Req()");
		if(nullptr == session->GetComponent<Component::UserData>())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		std::shared_ptr<Component::Dungeon::Data> dungeon = session->GetComponent<Component::Dungeon::Data>();
		if(nullptr == dungeon)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		std::shared_ptr<Unit> player = dungeon->player;
		AStarPathFinder pathFinder(dungeon, player->position, Vector2Int(req.destination.x, req.destination.y));
		for(const auto& point : pathFinder.path)
		{
			player->SetPosition(point);

			if(point != player->position)
			{
				break;
			}
			for(auto& itr : dungeon->monster)
			{
				std::shared_ptr<Unit> monster = itr.second;
				auto data = monster->attributes->GetComponent<Component::Monster::Data>();

				data->meta->Update(monster);
				Message::Vector2Int position;
				position.x = monster->position.x;
				position.y = monster->position.y;
				ans.monster_moves[monster->seq].push_back(position);
			}
			Message::Vector2Int position;
			position.x = player->position.x;
			position.y = player->position.y;
			ans.path.push_back(position);
		}
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (Message::ErrorCode)e.error_code();
	}
	LOG(DEV, "Message::Dungeon::MsgSvrCli_PlayerMove_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::Dungeon::MsgCliSvr_PlayerMove_Req,
	Handler_PlayerMove, Recv_Req,
	HandlerStatic
);

void Test_PlayerMove_Req(const std::shared_ptr<TestSession>& session)
{
	Message::Dungeon::MsgCliSvr_PlayerMove_Req req;
	Gamnet::Test::SendMsg(session, req);
}

void Test_PlayerMove_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Dungeon::MsgSvrCli_PlayerMove_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_PlayerMoveLobby_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Dungeon_PlayerMove",
	Message::Dungeon::MsgCliSvr_PlayerMove_Req, Test_PlayerMove_Req,
	Message::Dungeon::MsgSvrCli_PlayerMove_Ans, Test_PlayerMove_Ans
);

}}