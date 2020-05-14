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

	recv_packet = Packet::Create();
	if (nullptr == recv_packet)
	{
		LOG(GAMNET_ERR, "ErrorCode::NullPacketError can not create Packet instance");
		return false;
	}

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

void Session::AsyncSend(const std::shared_ptr<Packet> packet)
{
	if (true == packet->reliable)
	{
		auto self = std::static_pointer_cast<Session>(shared_from_this());
		strand->wrap([self](const std::shared_ptr<Packet> packet) {
			if(nullptr == self->socket)
			{
				return;
			}
			if (Session::RELIABLE_PACKET_QUEUE_SIZE <= self->send_packets.size())
			{
				self->handover_safe = false;
				self->Close(ErrorCode::SendQueueOverflowError);
				return;
			}
			self->send_packets.push_back(packet); // keep send message util ack received

			bool needFlush = self->send_buffers.empty();
			self->send_buffers.push_back(packet);
			if (true == needFlush)
			{
				self->FlushSend();
			}
		})(packet);
		return;
	}

	Network::Session::AsyncSend(packet);
}

void Session::OnRead(const std::shared_ptr<Buffer>& buffer) 
{
	try {
		while (0 < buffer->Size())
		{
			size_t readSize = std::min(buffer->Size(), recv_packet->Available());
			recv_packet->Append(buffer->ReadPtr(), readSize);
			buffer->Remove(readSize);

			while (Packet::HEADER_SIZE <= (int)recv_packet->Size())
			{
				recv_packet->ReadHeader();
				if (Packet::HEADER_SIZE > recv_packet->length)
				{
					throw GAMNET_EXCEPTION(ErrorCode::BufferUnderflowError, "buffer underflow(read size:", recv_packet->length, ")");
				}

				if (recv_packet->length >= (uint16_t)recv_packet->Capacity())
				{
					throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "buffer overflow(read size:", recv_packet->length, ")");
				}

				if (recv_packet->length > (uint16_t)recv_packet->Size())
				{
					break;
				}

				std::shared_ptr<Packet> packet = recv_packet;
				recv_packet = Packet::Create();
				if (nullptr == recv_packet)
				{
					throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create Packet instance");
				}
				recv_packet->Append(packet->ReadPtr() + packet->length, packet->Size() - packet->length);
				link_manager->OnRecvMsg(shared_from_this(), packet);
			}
		}
	}
	catch (const Exception& e)
	{
		if (nullptr != session)
		{
			std::static_pointer_cast<Session>(session)->handover_safe = false;
		}
		throw e;
	}
}

}}}