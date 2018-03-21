#include "Link.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network { namespace Tcp {

	Link::Link(Network::LinkManager* linkManager) : Network::Link(linkManager)
	{
	}

	Link::~Link()
	{
	}

	void Link::OnRead()
	{
		if (nullptr == session)
		{
			LOG(GAMNET_ERR, "invalid session(link_key:", link_key, ")");
			Close(ErrorCode::InvalidSessionError);
			return;
		}

		session->expire_time = ::time(nullptr);
		recv_packet->Append(read_buffer->ReadPtr(), read_buffer->Size());
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

			link_manager->OnRecvMsg(shared_from_this(), recv_packet);
			if (nullptr == session)
			{
				Close(ErrorCode::InvalidSessionError);
				return;
			}
			
			recv_packet->Remove(totalLength);

			if (0 < recv_packet->Size())
			{
				std::shared_ptr<Packet> packet = Packet::Create();
				if (nullptr == packet)
				{
					LOG(ERR, "can not create buffer(link_key:", link_key, ")");
					Close(ErrorCode::NullPacketError);
					return;
				}
				packet->Append(recv_packet->ReadPtr(), recv_packet->Size());
				recv_packet = packet;
			}
		}
	}
}}}