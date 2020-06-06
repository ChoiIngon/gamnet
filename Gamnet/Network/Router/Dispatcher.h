/*
 * Dispatcher.h
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_ROUTER_DISPATCHER_H_
#define GAMNET_NETWORK_ROUTER_DISPATCHER_H_

#include "../HandlerFactory.h"
#include "MsgRouter.h"
#include "../Tcp/Packet.h"

namespace Gamnet { namespace Network { namespace Router {

class Dispatcher
{
	typedef void(Network::IHandler::*function_type)(const Address&, const std::shared_ptr<Network::Tcp::Packet>&);
public :

	struct HandlerFunctor
	{
		Network::IHandlerFactory* factory_;
		std::function<void(const std::shared_ptr<Network::IHandler>&, const Address&, const std::shared_ptr<Network::Tcp::Packet>&)> function_;
	};
	std::map<uint32_t, HandlerFunctor> handler_functors;

public:
	Dispatcher();
	~Dispatcher();

	template <class FUNC, class FACTORY>
	bool BindHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		HandlerFunctor handlerFunction = { factory, (function_type)func };
		if(false == handler_functors.insert(std::make_pair(msg_id, handlerFunction)).second)
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateMessageIDError, "duplicate handler(msg_id:", msg_id, ")");
		}
		return true;
	}

	void OnReceive(const Address& from, const std::shared_ptr<Network::Tcp::Packet>& packet);
};
} /* namespace Router */
} /* namespace Gamnet */
}

#endif /* DISPATCHER_H_ */
