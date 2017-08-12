#include "Session.h"

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
}




