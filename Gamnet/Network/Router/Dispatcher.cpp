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

	void Dispatcher::OnReceive(const std::shared_ptr<Session>& session, const MsgRouter_SendMsg_Ntf& ntf)
	{
		std::shared_ptr<Network::Tcp::Packet> inner = Network::Tcp::Packet::Create();
		if (nullptr == inner)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create packet");
		}

		inner->Append(ntf.buffer.data(), ntf.buffer.size());
		inner->ReadHeader();

		Address addr = session->router_address;
		addr.msg_seq = inner->msg_seq;

		if (0 == ntf.msg_seq) // no need to be thread safe
		{
			auto itr = handler_functors.find(inner->msg_id);
			if (handler_functors.end() == itr)
			{
				LOG(GAMNET_ERR, "can't find handler function(msg_id:", inner->msg_id, ")");
				return;
			}

			std::shared_ptr<IHandlerFunctor> handlerFunctor = itr->second;
			
			thread_pool.PostTask(std::bind(&IHandlerFunctor::OnReceive, handlerFunctor, addr, inner));
			/*
			try {
				handlerFunctor->OnReceive(addr, inner);
			}
			catch (const std::exception& e)
			{
				LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
			}
			*/
		}
		else
		{
			auto itr = session->async_responses.find(ntf.msg_seq);
			if (session->async_responses.end() == itr)
			{
				return;
			}

			std::shared_ptr<Tcp::IAsyncResponse> response = itr->second;
			session->async_responses.erase(itr);

			response->StopTimer();
			
			thread_pool.PostTask(std::bind(&Tcp::IAsyncResponse::OnReceive, response, inner));
			//response->OnReceive(inner);
		}
	}
}}}