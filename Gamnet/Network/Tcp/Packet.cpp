/*
 * Packet.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#include <mutex>
#include "Packet.h"
#include "../../Library/Pool.h"
#include <algorithm>
//#include <atomic>

//#include <Windows.h>
//#include <DbgHelp.h>
//#include <Winbase.h>

namespace Gamnet { namespace Network { namespace Tcp {

	static Pool<Packet, std::mutex, Packet::Init, Packet::Release> packetPool_(65535);
	std::shared_ptr<Packet> Packet::Create()
	{
		return packetPool_.Create();
	}
	size_t Packet::PoolSize()
	{
		return packetPool_.Size();
	}
	size_t Packet::PoolAvailable()
	{
		return packetPool_.Available();
	}
	size_t Packet::PoolCapacity()
	{
		return packetPool_.Capacity();
	}
	
	Packet* Packet::Init::operator() (Packet* packet)
	{
		packet->Clear();
		packet->length = 0;
		packet->msg_seq = 0;
		packet->msg_id = 0;
		packet->reliable = false;
		return packet;
	};

	Packet* Packet::Release::operator() (Packet* packet)
	{
		return packet;
	}
	
	Packet::Packet() 
		: Buffer(Buffer::MAX_SIZE)
		, length(0)
		, msg_seq(0)
		, msg_id(0)
		, reliable(false)
	{
	}

	Packet::~Packet()
	{
	}

	bool Packet::Write(uint32_t msgID, const char* buf, size_t bytes)
	{
		Clear();
		length = (uint16_t)(HEADER_SIZE + bytes);
		msg_id = msgID;

		if (false == WriteHeader())
		{
			return false;
		}

		if (nullptr != buf && 0 < bytes)
		{
			std::memcpy(data + HEADER_SIZE, buf, bytes);
		}
		this->write_index += length;
		return true;
	}

	bool Packet::WriteHeader()
	{
		if (Capacity() <= length)
		{
			LOG(GAMNET_WRN, "packet max capacity over(msg_id:", msg_id, ", size:", length, ")");
			return false;
		}

		if ((uint16_t)Available() < length)
		{
			Resize(length);
		}

		(*(uint16_t*)(data + /*write_index +*/ OFFSET_LENGTH)) = length;
		(*(uint32_t*)(data + /*write_index +*/OFFSET_MSGSEQ)) = msg_seq;
		(*(uint32_t*)(data + /*write_index +*/OFFSET_MSGID)) = msg_id;
		(*(uint8_t*)(data + /*write_index +*/OFFSET_RELIABLE)) = reliable;
		(*(uint8_t*)(data + /*write_index +*/OFFSET_RESERVED)) = 0;

		return true;
	}

	bool Packet::ReadHeader()
	{
		length = *((uint16_t*)(data + read_index + OFFSET_LENGTH));
		msg_seq = *((uint32_t*)(data + read_index + OFFSET_MSGSEQ));
		msg_id = *((uint32_t*)(data + read_index + OFFSET_MSGID));
		reliable = *((uint8_t*)(data + read_index + OFFSET_RELIABLE));
		return true;
	}

	Packet::Protocol::Protocol()
		: recv_packet(nullptr)
	{
	}

	void Packet::Protocol::Init()
	{
		recv_packet = Packet::Create();
	}

	void Packet::Protocol::Clear()
	{
		recv_packet = nullptr;
		recv_packet_queue.clear();
	}

	void Packet::Protocol::Parse(const std::shared_ptr<Buffer>& buffer)
	{
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
				recv_packet_queue.push_back(packet);
			}
		}
	}

	std::shared_ptr<Packet> Packet::Protocol::Pop()
	{
		if(0 == recv_packet_queue.size())
		{
			return nullptr;
		}
		
		std::shared_ptr<Packet> packet = recv_packet_queue.front();
		recv_packet_queue.pop_front();
		return packet;
	}
}}}
