/*
 * Packet.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#include <mutex>
#include "Packet.h"
#include "../../Library/Pool.h"


namespace Gamnet { namespace Network { namespace Tcp {

	static Pool<Packet, std::mutex, Packet::Init> packetPool_(65535);
	std::shared_ptr<Packet> Packet::Create()
	{
		return packetPool_.Create();
	}
	
	Packet::Packet() : Buffer(Buffer::MAX_SIZE)
	{
	}

	Packet::~Packet()
	{
	}

	uint16_t Packet::GetLength() const
	{
		return *((uint16_t*)(data + readCursor_ + OFFSET_LENGTH));
	}

	uint32_t Packet::GetSEQ() const
	{
		return *((uint32_t*)(data + readCursor_ + OFFSET_MSGSEQ));
	}

	uint32_t Packet::GetID() const
	{
		return  *((uint32_t*)(data + readCursor_ + OFFSET_MSGID));
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

		(*(uint16_t*)(data + OFFSET_LENGTH)) = length;
		(*(uint32_t*)(data + OFFSET_MSGSEQ)) = msg_seq;
		(*(uint32_t*)(data + OFFSET_MSGID)) = msg_id;
		(*(uint8_t*)(data + OFFSET_RELIABLE)) = reliable;
		(*(uint8_t*)(data + OFFSET_RESERVED)) = 0;

		return true;
	}

	bool Packet::ReadHeader()
	{
		length = *((uint16_t*)(data + readCursor_ + OFFSET_LENGTH));
		msg_seq = *((uint32_t*)(data + readCursor_ + OFFSET_MSGSEQ));
		msg_id = *((uint32_t*)(data + readCursor_ + OFFSET_MSGID));
		reliable = *((uint8_t*)(data + readCursor_ + OFFSET_RELIABLE));
		return true;
	}
}}}
