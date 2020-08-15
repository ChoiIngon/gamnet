#include "UserSession.h"
#include "Component/Counter.h"
#include "Component/Bag.h"
#include "Component/Event.h"
#include "Component/Mail.h"

UserSession::UserSession()
	: account_type(AccountType::Invalid)
	, account_level(0)
	, shard_index(0)
	, user_seq(0)
	, create_date(Gamnet::Time::DateTime::MinValue)
{
}

UserSession::~UserSession()
{

}

void UserSession::OnCreate()
{
	auto self = std::static_pointer_cast<UserSession>(shared_from_this());

	AddComponent<Component::Counter>(std::make_shared<Component::Counter>(self));
	AddComponent<Component::Bag>(std::make_shared<Component::Bag>(self));
	AddComponent<Component::Event>(std::make_shared<Component::Event>(self));
	AddComponent<Component::Mail>(std::make_shared<Component::Mail>(self));
}

void UserSession::OnAccept()
{
	//LOG(INF, "[session_key:", session_key, "] OnAccept");
}

void UserSession::OnClose(int reason)
{
	//LOG(INF, "[session_key:", session_key, "] OnClose");
}

void UserSession::OnDestroy()
{
	account_id = "";
	account_type = AccountType::Invalid;
	account_level = 0;
	shard_index = 0;
	user_seq = 0;
	create_date = Gamnet::Time::DateTime::MinValue;
	components.Clear();
}

void UserSession::Commit()
{
	queries->Commit();
	logs->Commit();
	for(auto& itr : on_commit)
	{
		itr.second();
	}
	on_commit.clear();
}

void TestSession::OnCreate()
{
}

void TestSession::OnConnect()
{
}

void TestSession::OnClose(int reason)
{
}

void TestSession::OnDestroy()
{
	components.Clear();
}

