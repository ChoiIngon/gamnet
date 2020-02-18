/*
 * Dispatcher.h
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_ROUTER_DISPATCHER_H_
#define GAMNET_NETWORK_ROUTER_DISPATCHER_H_

#include "../Tcp/Tcp.h"
#include "MsgRouter.h"

namespace Gamnet { namespace Network { namespace Router {

class Dispatcher
{
	typedef void(Network::IHandler::*function_type)(const Address&, const std::shared_ptr<Network::Tcp::Packet>&);
public :

	struct HandlerFunction
	{
		Network::IHandlerFactory* factory_;
		std::function<void(const std::shared_ptr<Network::IHandler>&, const Address&, const std::shared_ptr<Network::Tcp::Packet>&)> function_;
	};
	std::map<unsigned int, HandlerFunction> mapHandlerFunction_;

public:
	Dispatcher();
	~Dispatcher();

	template <class FUNC, class FACTORY>
	bool BindHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		HandlerFunction handlerFunction = { factory, (function_type)func };
		if(false == mapHandlerFunction_.insert(std::make_pair(msg_id, handlerFunction)).second)
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateMessageIDError, "duplicate handler(msg_id:", msg_id, ")");
		}
		return true;
	}

	void OnRecvMsg(const Address& from, const std::shared_ptr<Network::Tcp::Packet>& packet);
};
} /* namespace Router */
} /* namespace Gamnet */
}

#endif /* DISPATCHER_H_ */
