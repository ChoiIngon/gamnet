#include "Link.h"
#include "LinkManager.h"

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

		msg_seq = 0;
		recv_packet = Packet::Create();
		if (nullptr == recv_packet)
		{
			LOG(GAMNET_ERR, "[link_key:", link_key, "] can not create recv packet");
			return false;
		}
		return true;
	}

	void Link::OnRead(const std::shared_ptr<Buffer>& buffer)
	{
		recv_packet->Append(buffer->ReadPtr(), buffer->Size());
		while (Packet::HEADER_SIZE <= (int)recv_packet->Size())
		{
			uint16_t totalLength = recv_packet->GetLength();
			if (Packet::HEADER_SIZE > totalLength)
			{
				LOG(GAMNET_ERR, "buffer underflow(read size:", totalLength, ")");
				Close(ErrorCode::BufferUnderflowError);
				return;
			}

			if (totalLength >= recv_packet->Capacity())
			{
				LOG(GAMNET_ERR, "buffer overflow(read size:", totalLength, ")");
				Close(ErrorCode::BufferOverflowError);
				return;
			}

			if (totalLength > (uint16_t)recv_packet->Size())
			{
				break;
			}

			std::shared_ptr<Packet> packet = recv_packet;
			recv_packet = Packet::Create();
			recv_packet->Append(packet->ReadPtr() + totalLength, packet->Size() - totalLength);
			link_manager->OnRecvMsg(shared_from_this(), packet);
		}
	}
}}}
