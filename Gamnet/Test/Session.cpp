/*
 * TestSession.cpp
 *
 *  Created on: Jun 7, 2014
 *      Author: kukuta
 */

#include "Session.h"

#include "LinkManager.h"
namespace Gamnet { namespace Test {

Session::Session() : test_seq(-1) {
}

Session::~Session() {
}

int	Session::Send(const std::shared_ptr<Network::Tcp::Packet>& packet)
{
	msg_seq++;
	AsyncSend(packet);
	return packet->Size();
}

int Session::Send(const char* buf, int len)
{
	msg_seq++;
	AsyncSend(buf, len);
	return len;
}

}}/* namespace Gamnet */
