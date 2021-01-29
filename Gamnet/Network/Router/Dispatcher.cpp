#include "Dispatcher.h"
#include "RouterCaster.h"
#include "../../Library/Singleton.h"
#include "../../Library/ThreadPool.h"

namespace Gamnet { namespace Network { namespace Router {

	static ThreadPool thread_pool(std::thread::hardware_concurrency());

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
		addr.msg_seq = packet->msg_seq;

		//thread_pool.PostTask(std::bind(&IHandlerFunctor::OnReceive, handlerFunctor, addr, packet));
		
		try {
			handlerFunctor->OnReceive(addr, packet);
		}
		catch (const std::exception& e)
		{
			LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
		}
	}
}}}