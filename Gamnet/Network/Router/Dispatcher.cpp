#include "Dispatcher.h"
#include "RouterCaster.h"
#include "../../Library/Singleton.h"

namespace Gamnet { namespace Network { namespace Router {
	
	Dispatcher::HandlerFunctor::HandlerFunctor() 
	{
	}

	Dispatcher::Dispatcher() 
	{
	}

	Dispatcher::~Dispatcher() 
	{
	}

	void Dispatcher::OnReceive(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet)
	{
		auto itr = handler_functors.find(packet->msg_id);
		if (handler_functors.end() == itr)
		{
			LOG(GAMNET_ERR, "can't find handler function(msg_id:", packet->msg_id, ")");
			return;
		}

		std::shared_ptr<HandlerFunctor> handlerFunctor = itr->second;
		std::shared_ptr<Network::IHandler> handler = nullptr;
		if (Network::IHandlerFactory::HANDLER_FACTORY_FIND == handlerFunctor->factory_->GetFactoryType())
		{
			/*
			std::shared_ptr<Session::ResponseTimeout> timeout = session->FindResponseTimeout(packet->msg_seq);
			if(nullptr == timeout)
			{
				LOG(GAMNET_ERR, "can't find wait response(msg_seq:", packet->msg_seq, ")");
				return;
			}

			handler = handlerFunctor->factory_->GetHandler(&session->handler_container, packet->msg_seq);
			*/
		}
		else
		{
			handler = handlerFunctor->factory_->GetHandler(nullptr, packet->msg_id);
		}

		if (nullptr == handler)
		{
			LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", packet->msg_seq, ", msg_id:", packet->msg_id, ")");
			return;
		}
		try {
			handlerFunctor->function_(handler, session->router_address, packet);
		}
		catch (const std::exception& e)
		{
			LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
		}
	}
}}}