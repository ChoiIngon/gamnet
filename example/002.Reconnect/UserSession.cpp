#include "UserSession.h"

UserSession::UserSession()
{
}

UserSession::~UserSession()
{
}

void UserSession::OnCreate()
{
	LOG(DEV, "session_key:", session_key);
}

void UserSession::OnAccept()
{
	LOG(DEV, "session_key:", session_key);
}

void UserSession::OnClose(int reason)
{
	LOG(DEV, "session_key:", session_key);
}

void UserSession::OnDestroy()
{
	LOG(DEV, "session_key:", session_key);
	components.Clear();
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

