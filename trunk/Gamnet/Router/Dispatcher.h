/*
 * Dispatcher.h
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_ROUTER_DISPATCHER_H_
#define GAMNET_ROUTER_DISPATCHER_H_

#include "../Network/Network.h"
namespace Gamnet {
namespace Router {
#include "MsgRouter.h"
class Dispatcher
{
	typedef void(Network::IHandler::*function_type)(const Address&, std::shared_ptr<Network::Packet>);
public :

	struct HandlerFunction
	{
		Network::IHandlerFactory* factory_;
		std::function<void(const std::shared_ptr<Network::IHandler>&, const Address&, std::shared_ptr<Network::Packet>)> function_;
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

	void OnRecvMsg(const Address& from, std::shared_ptr<Network::Packet> packet)
	{
		const unsigned int msg_id = packet->GetID();
		auto itr = mapHandlerFunction_.find(msg_id);
		if(itr == mapHandlerFunction_.end())
		{
			Log::Write(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ")");
			return ;
		}

		const HandlerFunction& handler_function = itr->second;

		if(ROUTER_CAST_UNI == from.cast_type && Network::IHandlerFactory::HANDLER_FACTORY_FIND == handler_function.factory_->GetFactoryType())
		{
			std::shared_ptr<Session> router_session = Singleton<RouterCaster>::GetInstance().FindSession(from);
			if(NULL == router_session)
			{
				Log::Write(GAMNET_ERR, "can't find registered address");
				return ;
			}

			std::shared_ptr<Network::Session> network_session = router_session->watingSessionManager_.FindSession(from.msg_seq);
			if(NULL == network_session)
			{
				Log::Write(GAMNET_ERR, "can't find session(msg_seq:", from.msg_seq, ")");
				return;
			}

			auto thread_safe_function = network_session->strand_.wrap([network_session, handler_function, msg_id, from, packet](){
				std::shared_ptr<Network::IHandler> handler = handler_function.factory_->GetHandler(&network_session->handlerContainer_, msg_id);
				if(NULL == handler)
				{
					Log::Write(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", msg_id, ")");
					return ;
				}
				//(handler.get()->*handler_function.function_)(from, packet);
				try {
					handler_function.function_(handler, from, packet);
				}
				catch (const std::exception& e)
				{
					Log::Write(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
				}
			});
			thread_safe_function();
		}
		else
		{
			std::shared_ptr<Network::IHandler> handler = handler_function.factory_->GetHandler(NULL, msg_id);
			if(NULL == handler)
			{
				Log::Write(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", msg_id, ")");
				return ;
			}

			//(handler.get()->*handler_function.function_)(from, packet);
			try {
				handler_function.function_(handler, from, packet);
			}
			catch (const std::exception& e)
			{
				Log::Write(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
			}
		}
	}
};
} /* namespace Router */
} /* namespace Gamnet */

#endif /* DISPATCHER_H_ */
