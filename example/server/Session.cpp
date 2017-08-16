#include "Session.h"
#include "Manager_Session.h"

Session::Session()
{

}
Session::~Session()
{

}

void Session::OnAccept()
{
	LOG(DEV, "session connected(session_key:", sessionKey_, ")");
	user_data = std::shared_ptr<UserData>(NULL);
}

void Session::OnClose(int reason)
{
	LOG(DEV, "session closed(session_key:", sessionKey_, ")");
	if(NULL == user_data)
	{
		return;
	}
	Gamnet::Singleton<Manager_Session>::GetInstance().Add(user_data->user_id, user_data);
}
