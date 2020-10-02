/*
 * Packet.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#include <mutex>
#include "Packet.h"
#include "../../Library/Pool.h"
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
}}}
