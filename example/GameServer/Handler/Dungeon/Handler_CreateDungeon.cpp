#include "Handler_CreateDungeon.h"
#include "../../Component/UserData.h"
#include "../../Component/UserData/Suit.h"
#include "../../Component/UserData/Item.h"
#include "../../Component/Dungeon/Dungeon.h"
#include "../../Component/Dungeon/Unit.h"
#include "../../Component/Dungeon/Unit/Monster.h"
#include "../../Component/Dungeon/Unit/Player.h"
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

		std::shared_ptr<Component::Unit::Data> player = Component::Unit::CreatePlayer(session, "Player1");
		std::shared_ptr<Component::Dungeon::Data> dungeon = Component::Dungeon::Enter(session, 1);

		Vector2Int start = dungeon->start->rect.Center();
		Vector2Int end = dungeon->end->rect.Center();

		ans.width = dungeon->GetRect().width;
		ans.height = dungeon->GetRect().height;
		for (auto tile : dungeon->GetAllTiles())
		{
			ans.tiles.push_back(tile->type);
		}
		
		ans.unit_seq = player->seq;
		ans.position = Message::Vector2Int(player->position.x, player->position.y);

		for(auto itr : dungeon->sessions)
		{
			std::shared_ptr<UserSession> user = itr.second;
			if(user == session)
			{
				continue;
			}

			std::shared_ptr<Component::Unit::Data> unit = user->GetComponent<Component::Unit::Data>();
			Message::Player comrade;
			comrade.unit_seq = unit->seq;
			comrade.position = Message::Vector2Int(unit->position.x, unit->position.y);
			std::shared_ptr<Component::Suit> suit = user->GetComponent<Component::Suit>();
			for (int part = (int)Message::EquipItemPartType::Invalid + 1; part < (int)Message::EquipItemPartType::Max; part++)
			{
				std::shared_ptr<Item::Data> item = suit->Find((Message::EquipItemPartType)part);
				if (nullptr == item)
				{
					continue;
				}

				comrade.equip_items.push_back(item->meta->index);
			}
			ans.comrades.push_back(comrade);
		}
		/*
		std::shared_ptr<Unit> monster = Gamnet::Singleton<Component::Monster::Manager>::GetInstance().CreateInstance(1);
		monster->dungeon = dungeon;
		monster->SetPosition(Vector2Int((int)end.x, (int)end.y));
		dungeon->monster.insert(std::make_pair(monster->seq, monster));
		*/
		
		for(auto itr : dungeon->monster)
		{
			std::shared_ptr<Component::Unit::Data> unit = itr.second;
			Message::Monster enemy;
			enemy.seq = unit->seq;
			enemy.position.x = unit->position.x;
			enemy.position.y = unit->position.y;
			ans.enemies.push_back(enemy);
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
  /*
	for(int y=0; y<ans.height; y++)
	{
		for(int x=0; x<ans.width; x++)
		{
			switch(ans.tiles[y * ans.width + x])
			{
			case Tile::Type::Door :
				std::cout << "+";
				break;
			case Tile::Type::StairDown :
				std::cout << "G";
				break;
			case Tile::Type::Wall :
				std::cout << "#";
				break;
			case Tile::Type::Floor:
				std::cout << ".";
				break;
			default :
				std::cout << " ";
				break;
			}
		}
		std::cout << std::endl;
	}
	*/
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Dungeon_CreateDungeon",
	Message::Dungeon::MsgCliSvr_CreateDungeon_Req, Test_CreateDungeon_Req,
	Message::Dungeon::MsgSvrCli_CreateDungeon_Ans, Test_CreateDungeon_Ans
);

}}