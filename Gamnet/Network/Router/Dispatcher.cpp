#include "Dispatcher.h"
#include "RouterCaster.h"
#include "../../Library/Singleton.h"

namespace Gamnet { namespace Network { namespace Router {
	
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

		std::shared_ptr<IHandlerFunctor> handlerFunctor = itr->second;
		
		Address addr = session->router_address;
		addr.session = session;
		addr.msg_seq = packet->msg_seq;

		try {
			handlerFunctor->OnReceive(addr, packet);
		}
		catch (const std::exception& e)
		{
			LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
		}
	}
}}}