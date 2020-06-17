#include "UserSession.h"

UserSession::UserSession()
{

}
UserSession::~UserSession()
{

}

void UserSession::OnCreate()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnCreate");
	Gamnet::Singleton<Manager_Session>::GetInstance().Add(std::static_pointer_cast<Gamnet::Network::Tcp::Session>(shared_from_this()));
}

void UserSession::OnAccept()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnAccept");
}

void UserSession::OnClose(int reason)
{
	//Gamnet::Singleton<Manager_Session>::GetInstance().Remove(user_data.UserID);
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnClose");
}

void UserSession::OnDestroy()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnDestory");
	Gamnet::Singleton<Manager_Session>::GetInstance().Remove(std::static_pointer_cast<Gamnet::Network::Tcp::Session>(shared_from_this()));
}

Manager_Session::Manager_Session() 
{
}

Manager_Session::~Manager_Session() 
{
}

void Manager_Session::Init()
{
	cast_group = Gamnet::Network::Tcp::CastGroup::Create();
}

GAMNET_BIND_INIT_HANDLER(Manager_Session, Init);

void Manager_Session::Add(std::shared_ptr<Gamnet::Network::Tcp::Session> session)
{
	Gamnet::Network::Tcp::CastGroup::LockGuard lo(cast_group);
	lo->Insert(session);
}

void Manager_Session::Remove(std::shared_ptr<Gamnet::Network::Tcp::Session> session)
{
	Gamnet::Network::Tcp::CastGroup::LockGuard lo(cast_group);
	lo->Remove(session);
}

void TestSession::OnCreate()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnCreate");
	pause_timer = Gamnet::Time::Timer::Create();
}

void TestSession::OnConnect()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnConnect");
}

void TestSession::OnClose(int reason)
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnClose");
	pause_timer = nullptr;
}

void TestSession::OnDestroy()
{
	//LOG(INF, "[", link->link_manager->name, "/", link->link_key, "/", session_key, "] OnDestroy");
}