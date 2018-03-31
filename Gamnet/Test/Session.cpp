#include "Session.h"

namespace Gamnet { namespace Test {

Session::Session() : test_seq(-1), is_pause(false) {
}

Session::~Session() {
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
