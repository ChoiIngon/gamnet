#include "UserSession.h"
#include "Component/UserData.h"
#include "Component/UserData/Item.h"
#include "Component/UserData/Bag.h"
#include "Component/UserData/Suit.h"
#include "Component/UserCounter.h"
#include "Component/Event.h"
#include "Component/Mail.h"
#include "Component/Disconnect.h"
#include "Component/Dungeon/Dungeon.h"
#include <idl/MessageLobby.h>

UserSession::UserSession()
	: user_no(0)
	, shard_index(0)
{
}

UserSession::~UserSession()
{
}

void UserSession::OnCreate()
{
	LOG(DEV, "OnCreate(session_key:", session_key, ")");
	auto self = std::static_pointer_cast<UserSession>(shared_from_this());
}

void UserSession::OnAccept()
{
	LOG(DEV, "OnAccept(session_key:", session_key, ")");
	auto disconnect = GetComponent<Component::Disconnect>();
	if(nullptr != disconnect) // reconnect
	{
		// information during disconnect
	}
	RemoveComponent<Component::Disconnect>();
}

void UserSession::OnClose(int reason)
{
	LOG(DEV, "OnClose(session_key:", session_key, ")");
	AddComponent<Component::Disconnect>();
}

void UserSession::OnDestroy()
{
	LOG(DEV, "OnDistroy(session_key:", session_key,")");
	Gamnet::Singleton<UserSession::Manager>::GetInstance().RemoveSession(std::static_pointer_cast<UserSession>(shared_from_this()));

	std::shared_ptr<Component::Dungeon::Data> dungeon = GetComponent<Component::Dungeon::Data>();
	if (nullptr != dungeon)
	{
		dungeon->Leave(std::static_pointer_cast<UserSession>(shared_from_this()));
	}
	
	components.Clear();

	user_no = 0;
	shard_index = 0;
}

void UserSession::StartTransaction()
{
	queries->Rollback();
	logs->Rollback();
	on_commit.clear();
}

void UserSession::Commit()
{
	queries->Commit();
	logs->Commit();
	for(auto& callback : on_commit)
	{
		callback();
	}
	on_commit.clear();
}

void UserSession::Manager::Init()
{
}

std::shared_ptr<UserSession> UserSession::Manager::AddSession(std::shared_ptr<UserSession> session)
{
	std::lock_guard<std::mutex> lo(lock);
	auto itr = sessions.find(session->user_no);
	if(sessions.end() == itr)
	{
		sessions.insert(std::make_pair(session->user_no, session));
		return nullptr;
	}

	std::shared_ptr<UserSession> prev = itr->second;
	sessions[session->user_no] = session;
	return prev;
}

void UserSession::Manager::RemoveSession(std::shared_ptr<UserSession> session)
{
	std::lock_guard<std::mutex> lo(lock);
	assert(0 != session->user_no);
	auto itr = sessions.find(session->user_no);
	if (sessions.end() == itr)
	{
		return;
	}

	std::shared_ptr<UserSession> exist = itr->second;
	if(exist == session)
	{
		sessions.erase(itr);
	}
}

GAMNET_BIND_INIT_HANDLER(UserSession::Manager, Init);

std::vector<int> TestSession::item_index = [](){
	std::vector<int> item_index;
	
	{
		MetaReader<Item::Meta> reader;
		auto& rows = reader.Read("../MetaData/Item.csv");
		for (auto& row : rows)
		{
			item_index.push_back(row->Index);
		}
	}

	{
		MetaReader<Item::Meta> reader;
		auto& rows = reader.Read("../MetaData/EquipItem.csv");
		for (auto& row : rows)
		{
			item_index.push_back(row->Index);
		}
	}
	
	return item_index;
}();

void TestSession::OnCreate()
{
	cheat_item_order = 0;
}

void TestSession::OnConnect()
{
}

void TestSession::OnClose(int reason)
{
}

void TestSession::OnDestroy()
{
	counters.clear();
	mails.clear();
	components.Clear();
}

