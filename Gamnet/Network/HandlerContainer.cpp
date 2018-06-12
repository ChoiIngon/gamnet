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
	mapHandler_.clear();
}

std::shared_ptr<IHandler> HandlerContainer::Find(uint32_t msg_id)
{
	auto itr = mapHandler_.find(msg_id);
	if(itr == mapHandler_.end())
	{
		return std::shared_ptr<IHandler>(NULL);
	}
	std::shared_ptr<IHandler> handler = itr->second;
	mapHandler_.erase(itr);
	return handler;
}

void HandlerContainer::Register(uint32_t msg_seq, std::shared_ptr<IHandler> handler)
{
	mapHandler_[msg_seq] = handler;
}

}} /* namespace Gamnet */
