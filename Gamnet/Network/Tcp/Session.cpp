#include "Session.h"

namespace Gamnet { namespace Network { namespace Tcp {

Session::Session()
{
}

Session::~Session()
{
}

int	Session::Send(const std::shared_ptr<Packet>& packet)
{
	AsyncSend(packet);
	return packet->Size();
}

int Session::Send(const char* buf, int len)
{
	AsyncSend(buf, len);
	return len;
}

}}}


