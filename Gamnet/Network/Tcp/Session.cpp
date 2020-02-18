#include "Session.h"
#include "Link.h"

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

	session_token = Session::GenerateSessionToken(session_key);

	recv_seq = 0;
	send_seq = 0;
	handover_safe = false;
	
	return true;
}

void Session::Clear()
{
	send_packets.clear();
	Network::Session::Clear();
}

bool Session::AsyncSend(const std::shared_ptr<Packet>& packet)
{
	if (true == packet->reliable)
	{
		link->strand.wrap([=]() {
			if (Session::RELIABLE_PACKET_QUEUE_SIZE <= this->send_packets.size())
			{
				this->handover_safe = false;
				link->Close(ErrorCode::SendQueueOverflowError);
				return;
			}

			send_packets.push_back(packet); // keep send message util ack received
			Network::Session::AsyncSend(packet);
		}) ();
		return true;
	}

	return Network::Session::AsyncSend(packet);
}

}}}