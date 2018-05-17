#include "Session.h"

namespace Gamnet { namespace Network { namespace Tcp {

Session::Session()
{
}

Session::Session(boost::asio::io_service& ioService) : Network::Session(ioService)
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

	session_token = Session::GenerateSessionToken(session_key);

	recv_seq = 0;
	send_seq = 0;
	//recv_packets.clear();
	send_packets.clear();
	return true;
}
}}}