#include "Session.h"

namespace Gamnet { namespace Network { namespace Tcp {

Session::Session()
{
}

Session::~Session()
{
}

bool Session::Init()
{
	if(false == Network::Session::Init())
	{
		return false;
	}
	msg_seq = 0;
	return true;;
}
}}}


