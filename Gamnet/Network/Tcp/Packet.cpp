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
}}}
