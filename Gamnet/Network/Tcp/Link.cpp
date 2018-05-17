#include "Link.h"
#include "LinkManager.h"
#include "Session.h"

namespace Gamnet { namespace Network { namespace Tcp {

	Link::Link(Network::LinkManager* linkManager) : Network::Link(linkManager)
	{
	}

	Link::~Link()
	{
	}

	bool Link::Init()
	{
		if(false == Network::Link::Init())
		{
			return false;
		}

//		recv_seq = 0;
		recv_packet = Packet::Create();
		if(nullptr == recv_packet)
		{
			LOG(GAMNET_ERR, "null packet instance");
			return false;
		}
		return true;
	}

	void Link::OnRead(const std::shared_ptr<Buffer>& buffer)
	{
		recv_packet->Append(buffer->ReadPtr(), buffer->Size());
		while (Packet::HEADER_SIZE <= (int)recv_packet->Size())
		{
			recv_packet->ReadHeader();
			if (Packet::HEADER_SIZE > recv_packet->length)
			{
				LOG(GAMNET_ERR, "buffer underflow(read size:", recv_packet->length, ")");
				Close(ErrorCode::BufferUnderflowError);
				return;
			}

			if (recv_packet->length >= recv_packet->Capacity())
			{
				LOG(GAMNET_ERR, "buffer overflow(read size:", recv_packet->length, ")");
				Close(ErrorCode::BufferOverflowError);
				return;
			}

			if (recv_packet->length > (uint16_t)recv_packet->Size())
			{
				break;
			}

			std::shared_ptr<Packet> packet = recv_packet;
			recv_packet = Packet::Create();
			if (nullptr == recv_packet)
			{
				LOG(GAMNET_ERR, "null packet instance");
				Close(ErrorCode::NullPacketError);
				return;
			}
			recv_packet->Append(packet->ReadPtr() + packet->length, packet->Size() - packet->length);
			link_manager->OnRecvMsg(shared_from_this(), packet);
		}
	}
}}}
