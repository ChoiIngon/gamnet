#include "Session.h"

namespace Gamnet { namespace Test {

Session::Session() : test_seq(-1), is_pause(false) {
}

Session::~Session() {
}

bool Session::Init()
{
	if(false == Network::Tcp::Session::Init())
	{
		return false;
	}
	test_seq = 0;
	is_pause = false;

	access_token = "";
	server_session_key = 0;
	return true;
}

void Session::Pause()
{
	is_pause = true;
}

void Session::Resume()
{
	is_pause = false;
}

}}/* namespace Gamnet */
