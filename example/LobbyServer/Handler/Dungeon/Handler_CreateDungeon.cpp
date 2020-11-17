#include "Handler_CreateDungeon.h"
#include "../../Component/UserData.h"
#include "../../Component/Dungeon/Dungeon.h"
#include "../../Component/Dungeon/Unit.h"
#include "../../Component/Dungeon/Unit/Monster.h"
#include <Gamnet/Library/Random.h>

namespace Handler { namespace Dungeon {

Handler_CreateDungeon::Handler_CreateDungeon()
{
}

Handler_CreateDungeon::~Handler_CreateDungeon()
{
}

void Handler_CreateDungeon::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Dungeon::MsgCliSvr_CreateDungeon_Req& req)
{
	Message::Dungeon::MsgSvrCli_CreateDungeon_Ans ans;
	ans.error_code = Message::ErrorCode::Success;
	
	try {
		LOG(DEV, "Message::Dungeon::MsgCliSvr_CreateDungeon_Req()");
		if(nullptr == session->GetComponent<Component::UserData>())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		std::shared_ptr<Component::Dungeon::Meta> meta = Gamnet::Singleton<Component::Dungeon::Manager>::GetInstance().FindMeta(1);
		std::shared_ptr<Component::Dungeon::Data> dungeon = meta->CreateInstance();
		session->AddComponent<Component::Dungeon::Data>(dungeon);
		dungeon->Init();

		Vector2 start = dungeon->start->rect.Center();
		std::shared_ptr<Unit> player = std::make_shared<Unit>();
		player->dungeon = dungeon;
		player->SetPosition(Vector2Int((int)start.x, (int)start.y));
		dungeon->player = player;

		/*
		Vector2 end = dungeon->end->rect.Center();
		std::shared_ptr<Unit> monster = Gamnet::Singleton<Component::Monster::Manager>::GetInstance().CreateInstance(1);
		monster->dungeon = dungeon;
		monster->SetPosition(Vector2Int((int)end.x, (int)end.y));
		dungeon->monster.insert(std::make_pair(monster->seq, monster));
		*/
		ans.width = dungeon->rect.width;
		ans.height = dungeon->rect.height;
		ans.start.x = dungeon->player->position.x;
		ans.start.y = dungeon->player->position.y;
		for(auto tile : dungeon->tiles)
		{
			ans.tiles.push_back(tile->type);
		}

		for(auto itr : dungeon->monster)
		{
			std::shared_ptr<Unit> monster = itr.second;
			Message::Monster msgMonster;
			msgMonster.seq = monster->seq;
			msgMonster.position.x = monster->position.x;
			msgMonster.position.y = monster->position.y;
			ans.monsters.push_back(msgMonster);
		}
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (Message::ErrorCode)e.error_code();
	}
	LOG(DEV, "Message::Dungeon::MsgSvrCli_CreateDungeon_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::Dungeon::MsgCliSvr_CreateDungeon_Req,
	Handler_CreateDungeon, Recv_Req,
	HandlerStatic
);

void Test_CreateDungeon_Req(const std::shared_ptr<TestSession>& session)
{
	Message::Dungeon::MsgCliSvr_CreateDungeon_Req req;
	Gamnet::Test::SendMsg(session, req);
}

void Test_CreateDungeon_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Dungeon::MsgSvrCli_CreateDungeon_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_CreateDungeonLobby_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	for(int y=0; y<ans.height; y++)
	{
		for(int x=0; x<ans.width; x++)
		{
			switch(ans.tiles[y * ans.width + x])
			{
			case Tile::Type::Door :
				std::cout << "D";
				break;
			case Tile::Type::StairDown :
				std::cout << "G";
				break;
			case Tile::Type::Wall :
				std::cout << "W";
				break;
			default :
				std::cout << " ";
				break;
			}
		}
		std::cout << std::endl;
	}

	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Dungeon_CreateDungeon",
	Message::Dungeon::MsgCliSvr_CreateDungeon_Req, Test_CreateDungeon_Req,
	Message::Dungeon::MsgSvrCli_CreateDungeon_Ans, Test_CreateDungeon_Ans
);

}}