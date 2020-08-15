#include "UserSession.h"

UserSession::UserSession()
{

}
UserSession::~UserSession()
{

}

void UserSession::OnCreate()
{
	//LOG(INF, "[session_key:", session_key, "] OnCreate");
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
	//LOG(INF, "[session_key:", session_key, "] OnDestroy");
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

int TestSession::GetCounter(uint32_t counterID)
{
	auto itr = counters.find(counterID);
	if (counters.end() == itr)
	{
		return 0;
	}
	return itr->second;
}

void TestSession::AddCounter(uint32_t counterID, int counter)
{
	auto itr = counters.find(counterID);
	if (counters.end() != itr)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError);
	}
	counters.insert(std::make_pair(counterID, counter));
}
