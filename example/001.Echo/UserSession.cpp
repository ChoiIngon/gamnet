#include "UserSession.h"

UserSession::UserSession()
{
}

UserSession::~UserSession()
{

}

void UserSession::OnCreate()
{
}

void UserSession::OnAccept()
{
}

void UserSession::OnClose(int reason)
{
}

void UserSession::OnDestroy()
{
	LOG(DEV, "UserSession destroy");
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
	//LOG(DEV, "[link_key:", link->link_key, "] socket destory");
}

