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
#include "../Tcp/Session.h"
#include "../../Library/Time/Timer.h"
#include <functional>

namespace Gamnet { namespace Network { namespace Router {

class Dispatcher
{
	typedef void(Network::IHandler::*function_type)(const Address&, const std::shared_ptr<Network::Tcp::Packet>&);
public :
	struct WaitResponse
	{
		time_t expire_time;
		std::function<void()> on_timeout;
		std::shared_ptr<Network::Tcp::Session> session;
	};
	
	class HandlerFunctor
	{
	private :
		Time::Timer expire_timer;
		std::mutex lock;
		std::map<uint32_t, std::shared_ptr<WaitResponse>> wait_responses;

	public :
		HandlerFunctor();

		Network::IHandlerFactory* factory_;
		std::function<void(const std::shared_ptr<Network::IHandler>&, const Address&, const std::shared_ptr<Network::Tcp::Packet>&)> function_;

		void RegisterWaitResponse(uint32_t msgSEQ, const std::shared_ptr<WaitResponse>& waitResponse);
		const std::shared_ptr<WaitResponse> FindWaitResponse(uint32_t msgSEQ);
	private :
		void OnTimeout();
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
	
	void RegisterWaitResponse(uint32_t msgID, uint32_t msgSEQ, const std::shared_ptr<WaitResponse>& waitResponse);
	void OnReceive(const Address& from, const std::shared_ptr<Network::Tcp::Packet>& packet);
};
} /* namespace Router */
} /* namespace Gamnet */
}

#endif /* DISPATCHER_H_ */
