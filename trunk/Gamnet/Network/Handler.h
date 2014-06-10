/*
 * Handler.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HANDLER_H_
#define GAMNET_NETWORK_HANDLER_H_

#include "Session.h"

namespace Gamnet { namespace Network {

struct IHandler : public std::enable_shared_from_this<IHandler>
{
	IHandler() {}
	virtual ~IHandler() {}

	virtual bool Init() { return true; }

	template <class MSG>
	bool SendMsg(std::shared_ptr<Session> session, const MSG& msg, unsigned int expect_id = 0)
	{
		if(0 != expect_id)
		{
			session->handlerContainer_.Register(expect_id, shared_from_this());
		}
		std::shared_ptr<Packet> packet = Packet::Create();
		if(NULL == packet)
		{
			return false;
		}
		if(false == packet->Write(msg))
		{
			return false;
		}
		if(0 > session->Send(packet))
		{
			return false;
		}
		return true;
	}
};

}} /* namespace Gamnet */
#endif /* HANDLER_H_ */
