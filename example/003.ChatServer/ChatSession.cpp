#include "ChatSession.h"

ChatSession::ChatSession()
{

}
ChatSession::~ChatSession()
{

}

void ChatSession::OnCreate()
{
	user_data.UserID = "";
	chat_channel = nullptr;
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnCreate");
}

void ChatSession::OnAccept()
{
	assert(nullptr != link);
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnAccept");
}

void ChatSession::OnClose(int reason)
{
	assert(nullptr != link);
	Gamnet::Singleton<Manager_Session>::GetInstance().Remove(user_data.UserID);
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnClose");
}

void ChatSession::OnDestroy()
{
	assert(nullptr != link);
	if(nullptr != chat_channel)
	{
		std::lock_guard<Gamnet::Network::Tcp::CastGroup> lo(*chat_channel);
		chat_channel->DelSession(std::static_pointer_cast<Gamnet::Network::Tcp::Session>(shared_from_this()));
		chat_channel = nullptr;
	}
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnDestory");
}

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

void TestSession::OnCreate()
{
	channel_seq = 0;
	chat_seq = 0;
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnCreate");
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
	user_ids.clear();
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnDestroy");
}