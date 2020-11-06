#include "UserSession.h"
#include "Component/UserData.h"
#include "Component/Counter.h"
#include "Component/Bag.h"
#include "Component/Event.h"
#include "Component/Mail.h"
#include "Component/Disconnect.h"

UserSession::UserSession()
	: shard_index(0)
	, user_seq(0)
{
}

UserSession::~UserSession()
{
}

void UserSession::OnCreate()
{
	LOG(DEV, "OnCreate(session_key:", session_key, ")");
	auto self = std::static_pointer_cast<UserSession>(shared_from_this());

	AddComponent<Component::Counter>(std::make_shared<Component::Counter>(self));
	AddComponent<Component::Bag>(std::make_shared<Component::Bag>(self));
	AddComponent<Component::Event>(std::make_shared<Component::Event>(self));
	AddComponent<Component::Mail>(std::make_shared<Component::Mail>(self));
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
	shard_index = 0;
	user_seq = 0;
	components.Clear();
}

void UserSession::StartTransaction()
{
	queries->Rollback();
	logs->Commit();
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
	counters.clear();
	mails.clear();
	components.Clear();
}

