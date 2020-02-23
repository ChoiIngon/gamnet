#include "UserSession.h"

UserSession::UserSession()
{

}
UserSession::~UserSession()
{

}

void UserSession::OnCreate()
{
	//LOG(DEV, "[session_key:", session_key, "] 'UserSession' is created");
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
	//LOG(DEV, "[link_key:", link->link_key, "] socket destroy");
}

void TestSession::OnCreate()
{
}

void TestSession::OnConnect()
{
}

void TestSession::OnClose(int reason)
{
	assert(nullptr != link);
}

void TestSession::OnDestroy()
{
	//LOG(DEV, "[link_key:", link->link_key, "] socket destory");
}

