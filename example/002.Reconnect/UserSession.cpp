#include "UserSession.h"

UserSession::UserSession()
{
}

UserSession::~UserSession()
{
}

void UserSession::OnCreate()
{
	LOG(INF, "[", link->link_key, "/", session_key, "] UserSession::OnCreate");
	component = std::make_shared<Gamnet::Component>();
}

void UserSession::OnAccept()
{
	LOG(INF, "[", link->link_key, "/", session_key, "] UserSession::OnAccept");
}

void UserSession::OnClose(int reason)
{
	LOG(INF, "[", link->link_key, "/", session_key, "] UserSession::OnClose");
}

void UserSession::OnDestroy()
{
	LOG(INF, "[", link->link_key, "/", session_key, "] UserSession::OnDestroy");
	component = nullptr;
}

void TestSession::OnCreate()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::OnCreate");
}

void TestSession::OnConnect()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::OnConnect");
}

void TestSession::OnClose(int reason)
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::OnClose");
}

void TestSession::OnDestroy()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::OnDestroy");
}

void TestSession::Reconnect()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::Reconnect");
	std::shared_ptr<Gamnet::Network::Tcp::Link> prevLink = std::static_pointer_cast<Gamnet::Network::Tcp::Link>(link);
	prevLink->session = nullptr;
	prevLink->Close(Gamnet::ErrorCode::UndefinedError);
	Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().Connect(std::static_pointer_cast<TestSession>(shared_from_this()));
}
