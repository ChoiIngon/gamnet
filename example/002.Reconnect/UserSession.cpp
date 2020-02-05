#include "UserSession.h"

UserSession::UserSession()
{
}

UserSession::~UserSession()
{
}

void UserSession::OnCreate()
{
	LOG(DEV, "[ServerSession] session_key:", session_key);
}

void UserSession::OnAccept()
{
	LOG(DEV, "[ServerSession] session_key:", session_key);
}

void UserSession::OnClose(int reason)
{
	LOG(DEV, "[ServerSession] session_key:", session_key);
}

void UserSession::OnDestroy()
{
	LOG(DEV, "[ServerSession] session_key:", session_key);
}

void TestSession::OnCreate()
{
	LOG(DEV, "[ClientSession] session_key:", server_session_key);
}

void TestSession::OnConnect()
{
	LOG(DEV, "[ClientSession] session_key:", server_session_key);
}

void TestSession::OnClose(int reason)
{
	LOG(DEV, "[ClientSession] session_key:", server_session_key);
}

void TestSession::OnDestroy()
{
	LOG(DEV, "[ClientSession] session_key:", server_session_key);
}

void TestSession::Reconnect()
{
	std::shared_ptr<Gamnet::Network::Link> prevLink = link;
	AttachLink(nullptr);
	prevLink->session = nullptr;
	prevLink->strand.wrap(std::bind(&Gamnet::Network::Link::Close, prevLink, Gamnet::ErrorCode::Success))();
	OnClose(0);

	std::shared_ptr<Gamnet::Network::Link> newLink = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().link_pool.Create();
	newLink->session = shared_from_this();
	AttachLink(newLink);

	const std::string& host = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().host;
	int port = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().port;
	link->Connect(host.c_str(), port, 5);
}
