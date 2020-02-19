#include "EchoSession.h"

EchoSession::EchoSession()
{

}
EchoSession::~EchoSession()
{

}

void EchoSession::OnCreate()
{
	//LOG(DEV, "[session_key:", session_key, "] 'EchoSession' is created");
}

void EchoSession::OnAccept()
{
	assert(nullptr != link);
}

void EchoSession::OnClose(int reason)
{
	assert(nullptr != link);
}

void EchoSession::OnDestroy()
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

