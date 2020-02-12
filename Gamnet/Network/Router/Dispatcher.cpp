#include "Dispatcher.h"
#include "RouterCaster.h"

namespace Gamnet { namespace Network { namespace Router {
	Dispatcher::Dispatcher() 
	{
	}

	Dispatcher::~Dispatcher() 
	{
	}

	void Dispatcher::OnRecvMsg(const Address& from, const std::shared_ptr<Network::Tcp::Packet>& packet)
	{
		const unsigned int msg_id = packet->msg_id;
		auto itr = mapHandlerFunction_.find(msg_id);
		if (itr == mapHandlerFunction_.end())
		{
			LOG(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ")");
			return;
		}

		const HandlerFunction& handler_function = itr->second;

		if (ROUTER_CAST_TYPE::UNI_CAST == from.cast_type && Network::IHandlerFactory::HANDLER_FACTORY_FIND == handler_function.factory_->GetFactoryType())
		{
			std::shared_ptr<Session> router_session = Singleton<RouterCaster>::GetInstance().FindSession(from);
			if (nullptr == router_session)
			{
				LOG(GAMNET_ERR, "can't find registered address");
				return;
			}

			std::shared_ptr<Network::Tcp::Session> network_session = router_session->watingSessionManager_.FindSession(from.msg_seq);
			if (nullptr == network_session)
			{
				LOG(GAMNET_ERR, "can't find session(msg_seq:", from.msg_seq, ")");
				return;
			}
			
			assert(nullptr != network_session->link);
			network_session->link->strand.wrap([=]() {
				std::shared_ptr<Network::IHandler> handler = handler_function.factory_->GetHandler(&network_session->handler_container, msg_id);
				if (nullptr == handler)
				{
					LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", msg_id, ")");
					return;
				}
				try {
					handler_function.function_(handler, from, packet);
				}
				catch (const std::exception& e)
				{
					LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
				}
			})();
		}
		else
		{
			std::shared_ptr<Network::IHandler> handler = handler_function.factory_->GetHandler(nullptr, msg_id);
			if (nullptr == handler)
			{
				LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", msg_id, ")");
				return;
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

}}}