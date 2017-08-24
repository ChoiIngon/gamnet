#include "Session.h"

namespace Gamnet { namespace Network { namespace Tcp {

Session::Session()
{
}

Session::~Session()
{
}

void Session::Send(const std::shared_ptr<Packet>& packet)
{
	AsyncSend(packet);
}

void Session::Send(const char* buf, int len)
{
	AsyncSend(buf, len);
}

void Session::Send(uint32_t msg_id, const Json::Value& json)
{
	Json::StyledWriter writer;
	std::string str = writer.write(json);

	Packet::Header header;
	header.msg_id = msg_id;
	header.msg_seq = msg_seq;
	header.length = (uint16_t)(Packet::HEADER_SIZE + str.length());

	std::shared_ptr<Packet> packet = Packet::Create();
	if(NULL == packet)
	{
		return;
	}
	packet->Write(header, str.c_str(), str.length());
	AsyncSend(packet);
}
}}}


