#include "Dispatcher.h"
#include "RouterCaster.h"
#include "../../Library/Singleton.h"

namespace Gamnet { namespace Network { namespace Router {
	
	Dispatcher::HandlerFunctor::HandlerFunctor() 
	{
	}

	void Dispatcher::HandlerFunctor::OnTimeout()
	{
		time_t now = time(nullptr);
		std::lock_guard<std::mutex> lo(lock);
		std::list<uint32_t> expiredMsgSEQs;
		for(auto& itr : wait_responses)
		{
			const std::shared_ptr<WaitResponse>& waitResponse = itr.second;
			if(waitResponse->expire_time < now)
			{
				uint32_t msgSEQ = itr.first;
				std::shared_ptr<Network::Tcp::Session> session = waitResponse->session;
				
				waitResponse->on_timeout();
				session->strand->wrap([session, msgSEQ](){
					session->handler_container.Find(msgSEQ);
				})();
				expiredMsgSEQs.push_back(msgSEQ);
			}
		}

		for(uint32_t msgSEQ : expiredMsgSEQs)
		{
			wait_responses.erase(msgSEQ);
		}

		if (0 == wait_responses.size())
		{
			expire_timer.Cancel();
		}
	}

	void Dispatcher::HandlerFunctor::RegisterWaitResponse(uint32_t msgSEQ, const std::shared_ptr<WaitResponse>& waitResponse)
	{
		std::lock_guard<std::mutex> lo(lock);
		wait_responses[msgSEQ] = waitResponse;
		if(1 == wait_responses.size())
		{
			expire_timer.AutoReset(true);
			expire_timer.SetTimer(1000, std::bind(&Dispatcher::HandlerFunctor::OnTimeout, this));
		}
	}

	const std::shared_ptr<Dispatcher::WaitResponse> Dispatcher::HandlerFunctor::FindWaitResponse(uint32_t msgSEQ)
	{
		std::lock_guard<std::mutex> lo(lock);
		std::shared_ptr<Dispatcher::WaitResponse> waitResponse = nullptr;
		auto itr = wait_responses.find(msgSEQ);
		if(wait_responses.end() != itr)
		{
			waitResponse = itr->second;
			wait_responses.erase(msgSEQ);
		}
		if (0 == wait_responses.size())
		{
			expire_timer.Cancel();
		}
		return waitResponse;
	}

	Dispatcher::Dispatcher() 
	{
	}

	Dispatcher::~Dispatcher() 
	{
	}

	void Dispatcher::RegisterWaitResponse(uint32_t msgID, uint32_t msgSEQ, const std::shared_ptr<WaitResponse>& waitResponse)
	{
		auto itr = handler_functors.find(msgID);
		if (handler_functors.end() == itr)
		{
			LOG(GAMNET_ERR, "can't find handler function(msg_id:", msgID, ")");
			return;
		}

		std::shared_ptr<HandlerFunctor> handlerFunctor = itr->second;
		handlerFunctor->RegisterWaitResponse(msgSEQ, waitResponse);
	}

	void Dispatcher::OnReceive(const Address& from, const std::shared_ptr<Network::Tcp::Packet>& packet)
	{
		auto itr = handler_functors.find(packet->msg_id);
		if (handler_functors.end() == itr)
		{
			LOG(GAMNET_ERR, "can't find handler function(msg_id:", packet->msg_id, ")");
			return;
		}

		std::shared_ptr<HandlerFunctor> handlerFunctor = itr->second;
		
		if (Network::IHandlerFactory::HANDLER_FACTORY_FIND == handlerFunctor->factory_->GetFactoryType())
		{
			std::shared_ptr<WaitResponse> waitResponse = handlerFunctor->FindWaitResponse(from.msg_seq);
			if(nullptr == waitResponse)
			{
				LOG(GAMNET_ERR, "can't find wait response(msg_seq:", from.msg_seq, ")");
				return;
			}

			std::shared_ptr<Network::Tcp::Session> session = waitResponse->session;
			if (nullptr == session)
			{
				LOG(GAMNET_ERR, "can't find session(msg_seq:", from.msg_seq, ")");
				return;
			}
			
			session->strand->wrap([=]() {
				std::shared_ptr<Network::IHandler> handler = handlerFunctor->factory_->GetHandler(&session->handler_container, from.msg_seq);
				if (nullptr == handler)
				{
					LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", packet->msg_id, ")");
					return;
				}
				try {
					handlerFunctor->function_(handler, from, packet);
				}
				catch (const std::exception& e)
				{
					LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
				}
			})();
		}
		else
		{
			std::shared_ptr<Network::IHandler> handler = handlerFunctor->factory_->GetHandler(nullptr, packet->msg_id);
			if (nullptr == handler)
			{
				LOG(GAMNET_ERR, "can't find handler instance(msg_seq:", from.msg_seq, ", msg_id:", packet->msg_id, ")");
				return;
			}

			try {
				handlerFunctor->function_(handler, from, packet);
			}
			catch (const std::exception& e)
			{
				LOG(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
			}
		}
	}

}}}