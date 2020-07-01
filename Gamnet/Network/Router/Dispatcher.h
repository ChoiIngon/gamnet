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
#include "Session.h"
#include "../../Library/Time/Timer.h"
#include <functional>

namespace Gamnet { namespace Network { namespace Router {

class Dispatcher
{
	typedef void(Network::IHandler::*function_type)(const Address&, const std::shared_ptr<Network::Tcp::Packet>&);
public :
	class HandlerFunctor
	{
	public :
		HandlerFunctor();

		Network::IHandlerFactory* factory_;
		std::function<void(const std::shared_ptr<Network::IHandler>&, const Address&, const std::shared_ptr<Network::Tcp::Packet>&)> function_;
	};

	std::map<uint32_t, std::shared_ptr<HandlerFunctor>> handler_functors;
	
public:
	Dispatcher();
	~Dispatcher();

	template <class FUNC, class FACTORY>
	bool BindHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		std::shared_ptr<HandlerFunctor> handlerFunctor = std::make_shared<HandlerFunctor>();
		handlerFunctor->factory_ = factory;
		handlerFunctor->function_ = (function_type)func;
		if(false == handler_functors.insert(std::make_pair(msg_id, handlerFunctor)).second)
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateMessageIDError, "duplicate handler(msg_id:", msg_id, ")");
		}
		return true;
	}

	void OnReceive(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
};
} /* namespace Router */
} /* namespace Gamnet */
}

#endif /* DISPATCHER_H_ */
