#include "Session.h"

namespace Gamnet { namespace Test {

Session::Session() : test_seq(-1), is_pause(false) {
}

Session::~Session() {
}

bool Session::Init()
{
	if(false == Network::Session::Init())
	{
		return false;
	}
	test_seq = 0;
	is_pause = false;
	return true;
}

void Session::Pause()
{
	is_pause = true;
}

void Session::Resume()
{
	is_pause = false;
	test_seq++;
}

}}/* namespace Gamnet */
