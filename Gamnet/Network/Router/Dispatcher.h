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
	Dispatcher() {}
	~Dispatcher() {}

	template <class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		HandlerFunction handlerFunction = { factory, (function_type)func };
		if(false == mapHandlerFunction_.insert(std::make_pair(msg_id, handlerFunction)).second)
		{
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] duplicate handler(msg_id:", msg_id, ")");
		}
		return true;
	}

	void OnRecvMsg(const Address& from, const std::shared_ptr<Network::Tcp::Packet>& packet)
	{
		const unsigned int msg_id = packet->GetID();
		auto itr = mapHandlerFunction_.find(msg_id);
		if(itr == mapHandlerFunction_.end())
		{
			LOG(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ")");
			return ;
		}

		const HandlerFunction& handler_function = itr->second;

		if(ROUTER_CAST_UNI == from.cast_type && Network::IHandlerFactory::HANDLER_FACTORY_FIND == handler_function.factory_->GetFactoryType())
		{
			std::shared_ptr<Session> router_session = Singleton<RouterCaster>::GetInstance().FindSession(from);
			if(NULL == router_session)
			{
				LOG(GAMNET_ERR, "can't find registered address");
				return ;
			}

			std::shared_ptr<Network::Session> network_session = router_session->watingSessionManager_.FindSession(from.msg_seq);
			if(NULL == network_session)
			{
				LOG(GAMNET_ERR, "can't find session(msg_seq:", from.msg_seq, ")");
				return;
			}

			{
				std::shared_ptr<Link> link = network_session->link.lock();
				auto thread_safe_function = link->strand.wrap([network_session, handler_function, msg_id, from, packet](){
					std::shared_ptr<Network::IHandler> handler = handler_function.factory_->GetHandler(&network_session->handler_container, msg_id);
					if(NULL == handler)
					{
						LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", msg_id, ")");
						return ;
					}
					try {
						handler_function.function_(handler, from, packet);
					}
					catch (const std::exception& e)
					{
						LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
					}
				});
				thread_safe_function();
			}
		}
		else
		{
			std::shared_ptr<Network::IHandler> handler = handler_function.factory_->GetHandler(NULL, msg_id);
			if(NULL == handler)
			{
				LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", msg_id, ")");
				return ;
			}

			try {
				handler_function.function_(handler, from, packet);
			}
			catch (const std::exception& e)
			{
				LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
			}
		}
	}
};
} /* namespace Router */
} /* namespace Gamnet */
}

#endif /* DISPATCHER_H_ */
