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
static Pool<Packet, std::mutex, Packet::Init> packetPool_(65535);

std::shared_ptr<Packet> Packet::Create()
{
	return packetPool_.Create();
}
}}
