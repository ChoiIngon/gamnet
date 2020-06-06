#include "UserSession.h"

UserSession::UserSession()
{

}
UserSession::~UserSession()
{

}

void UserSession::OnCreate()
{
	component = std::make_shared<Gamnet::Component>();
	chat_channel = nullptr;
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnCreate");
}

void UserSession::OnAccept()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnAccept");
}

void UserSession::OnClose(int reason)
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnClose");
}

void UserSession::OnDestroy()
{
	if(nullptr != chat_channel)
	{
		MsgSvrCli_LeaveChannel_Ntf ntf;

		std::shared_ptr<GUserData> userData = component->GetComponent<GUserData>();
		if (nullptr != userData)
		{
			ntf.LeaveUserData = *userData;
		}

		Gamnet::Network::Tcp::CastGroup::LockGuard lockedPtr(chat_channel);
		lockedPtr->Remove(std::static_pointer_cast<Gamnet::Network::Tcp::Session>(shared_from_this()));
		lockedPtr->SendMsg(ntf);
		chat_channel = nullptr;
	}
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnDestory");
	component = nullptr;
}

/*
Manager_Session::Manager_Session() {
}

Manager_Session::~Manager_Session() {
}

void Manager_Session::Init()
{
}

GAMNET_BIND_INIT_HANDLER(Manager_Session, Init);

const std::shared_ptr<ChatSession> Manager_Session::Add(const std::string& user_id, const std::shared_ptr<ChatSession>& session)
{
	std::lock_guard<std::mutex> lo(lock);
	std::shared_ptr<ChatSession> old_value = nullptr;
	auto itr = sessions.find(user_id);
	if (sessions.end() != itr)
	{
		old_value = itr->second;
	}
	sessions[user_id] = session;
	return old_value;
}

void Manager_Session::Remove(const std::string& user_id)
{
	std::lock_guard<std::mutex> lo(lock);
	sessions.erase(user_id);
}
*/

void TestSession::OnCreate()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnCreate");
	channel_seq = 0;
	recv_count = 0;
	user_ids.clear();
}

void TestSession::OnConnect()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnConnect");
}

void TestSession::OnClose(int reason)
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnClose");
}

void TestSession::OnDestroy()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnDestroy");
}