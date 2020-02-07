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
	cast_group = nullptr;
	//LOG(DEV, "[UserSession] session_key:", session_key);
}

void Session::OnAccept()
{
	assert(nullptr != link);
	//LOG(DEV, "[UserSession] session_key:", session_key, ", link_key:", link->link_key, ", link_manager:", link->link_manager->name);
}

void Session::OnClose(int reason)
{
	assert(nullptr != link);
	//LOG(DEV, "[UserSession] session_key:", session_key, ", link_key:", link->link_key, ", error_code:", reason);
	if(0 != reason)
	{
		LOG(ERR, "[UserSession] session_key:", session_key, ", link_key:", link->link_key, ", error_code:", reason);
	}
	Gamnet::Singleton<Manager_Session>::GetInstance().Remove(user_data.user_id);
}

void Session::OnDestroy()
{
	assert(nullptr == link);
	if(nullptr != cast_group)
	{
		cast_group->DelSession(std::static_pointer_cast<Gamnet::Network::Tcp::Session>(shared_from_this()));
		cast_group = nullptr;
	}
	//LOG(DEV, "[UserSession] session_key:", session_key);
}

void TestSession::OnCreate()
{
	user_data.user_id = "";
	user_data.user_seq = 0;
	//LOG(DEV, "[TestSession] session_key:", session_key);
}

void TestSession::OnConnect()
{
	host = remote_address->to_string();
	//LOG(DEV, "[TestSession] session_key:", session_key, ", link_key:", link->link_key, ", link_manager:", link->link_manager->name);
}

void TestSession::OnClose(int reason)
{
	//LOG(DEV, "[TestSession] session_key:", session_key, ", link_key:", link->link_key, ", error_code:", reason);
	Gamnet::Singleton<Manager_Session>::GetInstance().Remove(user_data.user_id);
}

void TestSession::OnDestroy()
{
	//LOG(DEV, "[TestSession] session_key:", session_key);
}

void TestSession::Reconnect()
{ 
	{
		std::shared_ptr<Gamnet::Network::Link> link = this->link;
		AttachLink(nullptr);
		link->Close(Gamnet::ErrorCode::Success);
	}

	std::shared_ptr<Gamnet::Network::Link> link = Gamnet::Singleton<Gamnet::Test::LinkManager<TestSession>>::GetInstance().Create();
	link->session = shared_from_this();
	AttachLink(link);
	
	link->Connect(host.c_str(), 20000, 5);
}
