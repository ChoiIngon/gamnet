#include "Session.h"

Session::Session()
	: shard_index(0)
	, user_no(0)
{
}

Session::~Session()
{
}

void Session::OnCreate()
{
}

void Session::OnAccept()
{
}

void Session::OnClose(int reason)
{
}

void Session::OnDestroy()
{
	shard_index = 0;
	user_no = 0;
	components.Clear();
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
	components.Clear();
}

