/*
 * HandlerContainer.cpp
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#include "HandlerContainer.h"

namespace Gamnet { namespace Network {

HandlerContainer::HandlerContainer() {
}

HandlerContainer::~HandlerContainer() {
}

void HandlerContainer::Init()
{
	handlers.clear();
}

std::shared_ptr<IHandler> HandlerContainer::Find(uint32_t msg_id)
{
	auto itr = handlers.find(msg_id);
	if(handlers.end() == itr)
	{
		return nullptr;
	}
	std::shared_ptr<IHandler> handler = itr->second;
	handlers.erase(itr);
	return handler;
}

void HandlerContainer::Register(uint32_t msg_seq, std::shared_ptr<IHandler> handler)
{
	handlers[msg_seq] = handler;
}

}} /* namespace Gamnet */
