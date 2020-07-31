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
		std::shared_ptr<Network::IHandler> handler = handlerFunctor->factory_->GetHandler(nullptr, packet->msg_id);
		if (nullptr == handler)
		{
			LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", packet->msg_seq, ", msg_id:", packet->msg_id, ")");
			return;
		}

		try {
			session->send_seq = packet->msg_seq;
				
			Address addr = session->router_address;
			addr.session = session;
			handlerFunctor->function_(handler, addr, packet);
		}
		catch (const std::exception& e)
		{
			LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
		}
	}
}}}