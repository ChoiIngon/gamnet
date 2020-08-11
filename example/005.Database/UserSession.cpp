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
}

std::shared_ptr<Counter> UserSession::GetCounter(uint32_t counterID)
{
	auto itr = counters.find(counterID);
	if(counters.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}

std::shared_ptr<Counter> UserSession::AddCounter(const std::shared_ptr<Counter>& counter)
{
	auto itr = counters.find(counter->counter_id);
	if (counters.end() != itr)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError);
	}
	counters.insert(std::make_pair(counter->counter_id, counter));
	return counter;
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
}

Counter::Counter(const std::shared_ptr<UserSession>& session, uint64_t counter_seq, uint32_t counter_id, int count)
	: session(session)
	, counter_seq(counter_seq)
	, counter_id(counter_id)
	, count(count)
{
}

int Counter::Increase(int amount)
{
	std::shared_ptr<UserData> userData = session->GetComponent<UserData>();
	if(nullptr == userData)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError);
	}
	count += amount;
	session->transaction->Update("user_counter", Gamnet::Format("counter=", count), {
		{ "counter_seq", counter_seq }
	});
	return count;
 }