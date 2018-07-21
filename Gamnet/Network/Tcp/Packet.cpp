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

		(*(uint16_t*)(data + write_index + OFFSET_LENGTH)) = length;
		(*(uint32_t*)(data + write_index + OFFSET_MSGSEQ)) = msg_seq;
		(*(uint32_t*)(data + write_index + OFFSET_MSGID)) = msg_id;
		(*(uint8_t*)(data + write_index + OFFSET_RELIABLE)) = reliable;
		(*(uint8_t*)(data + write_index + OFFSET_RESERVED)) = 0;

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
