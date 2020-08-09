#include "UserSession.h"

UserSession::UserSession()
{

}
UserSession::~UserSession()
{

}

void UserSession::OnCreate()
{
	//LOG(INF, "[session_key:", session_key, "] OnCreate");
}

void UserSession::OnAccept()
{
	//LOG(INF, "[session_key:", session_key, "] OnAccept");
}

void UserSession::OnClose(int reason)
{
	//LOG(INF, "[session_key:", session_key, "] OnClose");
}

void UserSession::OnDestroy()
{
	//LOG(INF, "[session_key:", session_key, "] OnDestroy");
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