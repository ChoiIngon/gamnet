#include "Session.h"
#include "Manager_Session.h"

Session::Session()
{

}
Session::~Session()
{

}

void Session::OnCreate()
{
	user_data.user_id = "";
	user_data.user_seq = 0;
	ack_seq = 0;
	LOG(DEV, "UserSession, link_key:", (nullptr == link ? 0 : link->link_key), ", session_key:", session_key);
}

void Session::OnAccept()
{
	LOG(DEV, "UserSession, link_key:", (nullptr == link ? 0 : link->link_key), ", session_key:", session_key);
}

void Session::OnClose(int reason)
{
	LOG(DEV, "UserSession, link_key:", (nullptr == link ? 0 : link->link_key), ", session_key:", session_key, ", error_code:", reason);
	Gamnet::Singleton<Manager_Session>::GetInstance().Remove(user_data.user_id);
}

void Session::OnDestroy()
{
	LOG(DEV, "UserSession, link_key:", (nullptr == link ? 0 : link->link_key), ", session_key:", session_key);
}

void TestSession::OnCreate()
{
	user_data.user_id = "";
	user_data.user_seq = 0;
	LOG(DEV, "TestSession, link_key:", (nullptr == link ? 0 : link->link_key), ", session_key:", session_key);
}

void TestSession::OnConnect()
{
	host = remote_address->to_string();
	LOG(DEV, "TestSession, link_key:", (nullptr == link ? 0 : link->link_key), ", session_key:", session_key, ", host:", host);
}

void TestSession::OnClose(int reason)
{
	LOG(DEV, "TestSession, link_key:", (nullptr == link ? 0 : link->link_key), ", session_key:", session_key, ", error_code:", reason);
	Gamnet::Singleton<Manager_Session>::GetInstance().Remove(user_data.user_id);
}

void TestSession::OnDestroy()
{
	LOG(DEV, "TestSession, link_key:", (nullptr == link ? 0 : link->link_key), ", session_key:", session_key);
}
void TestSession::Reconnect()
{ 
	{
		std::shared_ptr<Gamnet::Network::Link> link = this->link;
		this->link->AttachSession(nullptr);
		link->Close(Gamnet::ErrorCode::Success);
	}

	std::shared_ptr<Gamnet::Network::Link> link = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().Create();
	link->AttachSession(shared_from_this());
	link->Connect(host.c_str(), 20000, 5);
	Pause();
}
