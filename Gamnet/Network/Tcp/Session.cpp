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
	handover_safe = false;
	//recv_packets.clear();
	send_packets.clear();
	return true;
}

bool Session::AsyncSend(const std::shared_ptr<Packet>& packet)
{
	if (true == packet->reliable)
	{
		this->strand.wrap([this, packet](){
			packet->msg_seq = ++this->send_seq;
			packet->WriteHeader();
			if (Session::RELIABLE_PACKET_QUEUE_SIZE > send_packets.size())
			{
				send_packets.push_back(packet);
			}
			this->Network::Session::AsyncSend(packet);
		})();
		return true;
	}

	return Network::Session::AsyncSend(packet);
}

}}}