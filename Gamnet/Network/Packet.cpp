/*
 * Packet.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#include <mutex>
#include "Packet.h"
#include "../Library/Pool.h"


namespace Gamnet { namespace Network {

Packet::Packet() : Buffer(Buffer::MAX_SIZE)
{
}

Packet::~Packet()
{
}

uint32_t Packet::HeaderSize()
{
	return HEADER_SIZE;
}

uint16_t Packet::GetTotalLength() const
{
	return *((uint16_t*)(buf_ + LENGTH_PTR));
}
uint32_t Packet::GetID() const
{
	return  *((uint32_t*)(buf_ + MSGID_PTR));
}

static Pool<Packet, std::mutex, Packet::Init> packetPool_(65535);


std::shared_ptr<Packet> Packet::Create()
{
	return packetPool_.Create();
}
}}
