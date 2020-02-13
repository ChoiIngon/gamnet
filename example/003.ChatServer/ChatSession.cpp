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
	LOG(DEV, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "]");
}

void ChatSession::OnAccept()
{
	assert(nullptr != link);
	//LOG(DEV, "[UserChatSession] session_key:", session_key, ", link_key:", link->link_key, ", link_manager:", link->link_manager->name);
}

void ChatSession::OnClose(int reason)
{
	assert(nullptr != link);
	//LOG(DEV, "[UserChatSession] session_key:", session_key, ", link_key:", link->link_key, ", error_code:", reason);
	Gamnet::Singleton<Manager_Session>::GetInstance().Remove(user_data.UserID);
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
	//LOG(DEV, "[UserSession] session_key:", session_key);
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
	//LOG(DEV, "[TestSession] session_key:", session_key);
}

void TestSession::OnConnect()
{
	//LOG(DEV, "[TestSession] session_key:", session_key, ", link_key:", link->link_key, ", link_manager:", link->link_manager->name);
}

void TestSession::OnClose(int reason)
{
	//LOG(DEV, "[TestSession] session_key:", session_key, ", link_key:", link->link_key, ", error_code:", reason);
}

void TestSession::OnDestroy()
{
	user_ids.clear();
	//LOG(DEV, "[TestSession] session_key:", session_key);
}