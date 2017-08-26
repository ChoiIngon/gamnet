#include "Session.h"
#include "Manager_Session.h"

Session::Session()
{

}
Session::~Session()
{

}

void Session::OnCreate()
{
	user_data.user_id = "";
	user_data.user_seq = 0;
	ack_seq = 0;
}

void Session::OnAccept()
{
	LOG(DEV, "session connected(session_key:", session_key, ")");

}

void Session::OnClose(int reason)
{
	LOG(DEV, "session closed(session_key:", session_key, ")");
	if("" == user_data.user_id)
	{
		return;
	}
	Gamnet::Singleton<Manager_Session>::GetInstance().Remove(user_data.user_id);
}
