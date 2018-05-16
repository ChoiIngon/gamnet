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

		recv_seq = 0;
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
			if (nullptr == recv_packet)
			{
				LOG(GAMNET_ERR, "null packet instance");
				Close(ErrorCode::NullPacketError);
				return;
			}
			recv_packet->Append(packet->ReadPtr() + totalLength, packet->Size() - totalLength);
			link_manager->OnRecvMsg(shared_from_this(), packet);
		}
	}

	void Link::OnSend(const boost::system::error_code& ec, std::size_t transferredBytes)
	{
		if (0 != ec)
		{
			Close(ErrorCode::SendMsgFailError);
			return;
		}

		if (true == send_buffers.empty())
		{
			return;
		}

		if(nullptr != session)
		{
			std::shared_ptr<Packet> packet = std::static_pointer_cast<Packet>(send_buffers.front());
			if(true == packet->reliable)
			{
				std::shared_ptr<Session> tcpSession = std::static_pointer_cast<Session>(session);
				if(RELIABLE_PACKET_QUEUE_SIZE > tcpSession->send_packets.size())
				{
					tcpSession->send_packets.push_back(packet);
				}
			}			
		}

		send_buffers.pop_front();
		FlushSend();
	}
}}}
