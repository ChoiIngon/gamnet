#include "Dispatcher.h"
#include "RouterCaster.h"

namespace Gamnet { namespace Network { namespace Router {
	Dispatcher::Dispatcher() 
	{
	}

	Dispatcher::~Dispatcher() 
	{
	}

	void Dispatcher::OnReceive(const Address& from, const std::shared_ptr<Network::Tcp::Packet>& packet)
	{
		auto itr = handler_functors.find(packet->msg_id);
		if (handler_functors.end() == itr)
		{
			LOG(GAMNET_ERR, "can't find handler function(msg_id:", packet->msg_id, ")");
			return;
		}

		const HandlerFunctor& handlerFunctor = itr->second;

		if (ROUTER_CAST_TYPE::UNI_CAST == from.cast_type && Network::IHandlerFactory::HANDLER_FACTORY_FIND == handlerFunctor.factory_->GetFactoryType())
		{
			std::shared_ptr<Session> router_session = Singleton<RouterCaster>::GetInstance().FindSession(from);
			if (nullptr == router_session)
			{
				LOG(GAMNET_ERR, "can't find registered address");
				return;
			}

			std::shared_ptr<Network::Tcp::Session> network_session = router_session->wait_session_manager.FindSession(from.msg_seq);
			if (nullptr == network_session)
			{
				LOG(GAMNET_ERR, "can't find session(msg_seq:", from.msg_seq, ")");
				return;
			}
			
			network_session->strand->wrap([=]() {
				std::shared_ptr<Network::IHandler> handler = handlerFunctor.factory_->GetHandler(&network_session->handler_container, packet->msg_id);
				if (nullptr == handler)
				{
					LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", packet->msg_id, ")");
					return;
				}
				try {
					handlerFunctor.function_(handler, from, packet);
				}
				catch (const std::exception& e)
				{
					LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
				}
			})();
		}
		else
		{
			std::shared_ptr<Network::IHandler> handler = handlerFunctor.factory_->GetHandler(nullptr, packet->msg_id);
			if (nullptr == handler)
			{
				LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", packet->msg_id, ")");
				return;
			}

			try {
				handlerFunctor.function_(handler, from, packet);
			}
			catch (const std::exception& e)
			{
				LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
			}
		}
	}

}}}