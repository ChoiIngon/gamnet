/*
 * Dispatcher.h
 *
 *  Created on: Aug 7, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_TCP_DISPATCHER_H_
#define GAMNET_NETWORK_TCP_DISPATCHER_H_
#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include <set>
#include "Packet.h"
#include "Link.h"
#include "../Handler.h"
#include "../HandlerFactory.h"
#include "../../Log/Log.h"
#ifdef _DEBUG
#include <chrono>
#endif

namespace Gamnet { namespace Network { namespace Tcp {

template <class SESSION_T>
class Dispatcher
{
	GAMNET_WHERE(SESSION_T, Session);

	typedef void(IHandler::*function_type)(const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Packet>&);
public :
	struct HandlerFunction
	{
		template<class FACTORY, class FUNCTION>
#ifdef _WIN32
		HandlerFunction(FACTORY* factory, FUNCTION function) : factory_(factory), function_(std::mem_fn(function)) {}
#else
		HandlerFunction(FACTORY* factory, FUNCTION function) : factory_(factory), function_(function) {}
#endif
		IHandlerFactory* factory_;
		//function_type function_;
		std::function<void(const std::shared_ptr<IHandler>&, const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Packet>&)> function_;
	};

	std::map<unsigned int, HandlerFunction> mapHandlerFunction_;
#ifdef _DEBUG
	struct HandlerCallStatistics
	{
		HandlerCallStatistics() : msg_id(0), begin_count(0), finish_count(0), elapsed_time(0) {
		}
		unsigned int msg_id;
		const char* name;
		std::atomic_int begin_count;
		std::atomic_int finish_count;
		std::atomic<uint64_t> elapsed_time;
	};
	std::map<unsigned int, std::shared_ptr<HandlerCallStatistics>> mapHandlerCallStatistics_;
#endif
public:
	Dispatcher() {}
	~Dispatcher() {}

	template <class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, const char* name, FUNC func, FACTORY factory)
	{
		HandlerFunction handlerFunction(factory, static_cast<function_type>(func));
		if(false == mapHandlerFunction_.insert(std::make_pair(msg_id, handlerFunction)).second)
		{
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] duplicate handler(msg_id:", msg_id, ")");
		}

#ifdef _DEBUG
		std::shared_ptr<HandlerCallStatistics> statistics = std::shared_ptr<HandlerCallStatistics>(new HandlerCallStatistics());
		statistics->msg_id = msg_id;
		statistics->name = name;
		mapHandlerCallStatistics_.insert(std::make_pair(msg_id, statistics));
#endif
		return true;
	}

	void OnRecvMsg(const std::shared_ptr<SESSION_T> session, const std::shared_ptr<Packet> packet)
	{
		uint32_t msgSEQ = packet->GetSEQ();
		if (msgSEQ <= session->recv_seq)
		{
			LOG(WRN, "[session_key:", session->session_key, "] discard message(msg_id:", packet->GetID(), ", received msg_seq:", msgSEQ, ", expected msg_seq:", session->recv_seq + 1, ")");
			return;
		}

		LOG(DEV, "[session_key:", session->session_key, "] recv message(msg_id:", packet->GetID(), ", received msg_seq:", msgSEQ, ", expected msg_seq:", session->recv_seq + 1, ")");
		std::shared_ptr<Link> link = std::static_pointer_cast<Link>(session->link);

		const unsigned int msg_id = packet->GetID();
		auto itr = mapHandlerFunction_.find(msg_id);
		if(itr == mapHandlerFunction_.end())
		{
			LOG(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ", session_key:", session->session_key,",packet_size:", packet->Size(), ", packet_read_cursor:", packet->readCursor_, ")");
			link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::NullPointerError))();
			return ;
		}

		const HandlerFunction& handler_function = itr->second;
		std::shared_ptr<IHandler> handler = handler_function.factory_->GetHandler(&session->handler_container, msg_id);
		if(NULL == handler)
		{
			LOG(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ", session_key:", session->session_key,",packet_size:", packet->Size(), ", packet_read_cursor:", packet->readCursor_, ")");
			link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::NullPointerError))();
			return;
		}
#ifdef _DEBUG
		ElapseTimer diff;
		auto statistics_itr = mapHandlerCallStatistics_.find(msg_id);
		if (mapHandlerCallStatistics_.end() != statistics_itr)
		{
			statistics_itr->second->begin_count++;
		}
#endif
		try {
			handler_function.function_(handler, session, packet);
		}
		catch (const std::exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			link->strand.wrap(std::bind(&Network::Link::Close, link, ErrorCode::UndefinedError))();
			return;
		}

		/*
		if (msgSEQ > session->send_seq)
		{
			while (0 < session->send_packets.size())
			{
				const std::shared_ptr<Packet>& sendPacket = session->send_packets.front();
				if (msgSEQ < sendPacket->GetSEQ())
				{
					break;
				}
				LOG(DEV, "remove send packet(msg_seq:", sendPacket->GetSEQ(), ", msg_id:", sendPacket->GetID(), ")");
				session->send_packets.pop_front();
			}
			session->send_seq = msgSEQ;
		}
		*/
		session->recv_seq = msgSEQ;
#ifdef _DEBUG
		if (mapHandlerCallStatistics_.end() != statistics_itr)
		{
			statistics_itr->second->elapsed_time += diff.Count();
			statistics_itr->second->finish_count++;
		}
#endif
	}
};

}}}
#endif /* DISPATCHER_H_ */
