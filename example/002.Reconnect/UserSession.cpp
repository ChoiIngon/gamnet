#include "UserSession.h"

UserSession::UserSession()
{
}

UserSession::~UserSession()
{
}

void UserSession::OnCreate()
{
	LOG(DEV, "[session_key:", session_key, "] 'UserSession' is created");
}

void UserSession::OnAccept()
{
	assert(nullptr != link);
}

void UserSession::OnClose(int reason)
{
	assert(nullptr != link);
}

void UserSession::OnDestroy()
{
	LOG(DEV, "[session_key:", session_key, "] 'UserSession' is destroyed");
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

void TestSession::Reconnect()
{

	std::shared_ptr<Gamnet::Network::Link> prevLink = link;
	AttachLink(nullptr);
	prevLink->strand.wrap(std::bind(&Gamnet::Network::Link::Close, prevLink, Gamnet::ErrorCode::Success))();

	std::shared_ptr<Gamnet::Network::Link> newLink = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().link_pool.Create();
	newLink->session = shared_from_this();
	AttachLink(newLink);

	const std::string& host = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().host;
	int port = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().port;
	link->Connect(host.c_str(), port, 5);
}
