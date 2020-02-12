#include "UserSession.h"

UserSession::UserSession()
{
}

UserSession::~UserSession()
{
}

void UserSession::OnCreate()
{
	LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] UserSession::OnCreate");
}

void UserSession::OnAccept()
{
	LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] UserSession::OnAccept");
}

void UserSession::OnClose(int reason)
{
	LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] UserSession::OnClose");
}

void UserSession::OnDestroy()
{
	LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] UserSession::OnDestroy");
}

void TestSession::OnCreate()
{
	LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::OnCreate");
}

void TestSession::OnConnect()
{
	LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::OnConnect");
}

void TestSession::OnClose(int reason)
{
	LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::OnClose");
}

void TestSession::OnDestroy()
{
	LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::OnDestroy");
}

void TestSession::Reconnect()
{
	LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] TestSession::Reconnect");
	std::shared_ptr<Gamnet::Network::Link> prevLink = link;
	prevLink->session = nullptr;
	prevLink->Close(Gamnet::ErrorCode::UndefinedError);
	//AttachLink(nullptr);
	//OnClose(0);

	std::shared_ptr<Gamnet::Network::Link> newLink = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().link_pool.Create();
	AttachLink(newLink);
	newLink->session = shared_from_this();

	const std::string& host = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().host;
	int port = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().port;
	link->Connect(host.c_str(), port, 5);
}
