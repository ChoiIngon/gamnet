#include "Session.h"

#include "../../Library/MD5.h"
#include "../../Library/Random.h"
#include "../../Library/Time/Time.h"
#include "SessionManager.h"

namespace Gamnet { namespace Network { namespace Tcp {

std::string Session::GenerateSessionToken(uint32_t session_key)
{
	return md5(Format(session_key, time(nullptr), Random::Range(1, 99999999)));
}

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
	last_recv_time = time(nullptr);
	
	return true;
}

void Session::Clear()
{
	send_packets.clear();
	Network::Session::Clear();
}

void Session::AsyncSend(const std::shared_ptr<Packet>& packet)
{
	if (true == packet->reliable)
	{
		auto self = std::static_pointer_cast<Session>(shared_from_this());
		Dispatch([this, self, packet]() {
			if (Session::RELIABLE_PACKET_QUEUE_SIZE <= send_packets.size())
			{
				handover_safe = false;
				Close(ErrorCode::SendQueueOverflowError);
				return;
			}
			send_packets.push_back(packet); // keep send message util ack received
			bool needFlush = send_buffers.empty();
			send_buffers.push_back(packet);
			if (true == needFlush)
			{
				FlushSend();
			}
		});
		return;
	}

	Network::Session::AsyncSend(packet);
}

void Session::OnRead(const std::shared_ptr<Buffer>& buffer) 
{
	try {
		last_recv_time = time( nullptr );
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
				auto self = shared_from_this();
				session_manager->OnReceive(self, packet);
			}
		}
	}
	catch (const Exception& e)
	{
		handover_safe = false;
		throw e;
	}
}

void Session::Close(int reason)
{
	auto self(shared_from_this());
	Dispatch([this, self, reason]() {
		if (nullptr != socket)
		{
			OnClose(reason);
			socket = nullptr;
		}
		
		if(0 != session_key && false == handover_safe)
		{
			OnDestroy();
			session_manager->Remove(self);
			session_key = 0;
		}
	});
}

}}}