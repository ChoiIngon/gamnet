#include "Session.h"
#include "Link.h"

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
	return true;
}

void Session::Clear()
{
	send_packets.clear();
	Network::Session::Clear();
}

bool Session::AsyncSend(const std::shared_ptr<Packet>& packet)
{
	if(nullptr == link)
	{
		LOG(WRN, "[session_key:", session_key, "] invalid link(msg_id:", packet->msg_id, ")");
	}
	if (true == packet->reliable)
	{
		strand.wrap([=](){
			if (100 * Session::RELIABLE_PACKET_QUEUE_SIZE <= this->send_packets.size())
			{
				if(nullptr != this->link)
				{
					this->link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::NullPointerError))();
				}

				return;
			}
			send_packets.push_back(packet);
			this->Network::Session::AsyncSend(packet);
		})();
		return true;
	}

	return Network::Session::AsyncSend(packet);
}

}}}