/*
 * Dispatcher.h
 *
 *  Created on: Aug 7, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_DISPATCHER_H_
#define GAMNET_NETWORK_DISPATCHER_H_
#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include <set>
#include "../Library/Pool.h"
#include "Handler.h"
#include "HandlerFactory.h"
#include "../Log/Log.h"

namespace Gamnet { namespace Network {

template <class SESSION_T>
class Dispatcher
{
	typedef void(IHandler::*function_type)(std::shared_ptr<SESSION_T>, std::shared_ptr<Packet>);
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
		std::function<void(const std::shared_ptr<IHandler>&, std::shared_ptr<SESSION_T>, std::shared_ptr<Packet>)> function_;
	};

	std::map<unsigned int, HandlerFunction> mapHandlerFunction_;
#ifdef _DEBUG
	struct HandlerCallInfo 
	{
		HandlerCallInfo() : handlerID(0), beginCount(0), finishCount(0), elapsedTime(0.0f) {
		}
		int handlerID;
		std::atomic_int beginCount;
		std::atomic_int finishCount;
		std::atomic<std::int64_t> elapsedTime;
	};
	std::map<unsigned int, std::shared_ptr<HandlerCallInfo>> mapHandlerCallInfo_;
#endif
public:
	Dispatcher() {}
	~Dispatcher() {}

	template <class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		HandlerFunction handlerFunction(factory, static_cast<function_type>(func));
		if(false == mapHandlerFunction_.insert(std::make_pair(msg_id, handlerFunction)).second)
		{
#ifdef _WIN32
			throw Exception(0, "[", __FILE__, ":", __FUNCTION__, "@", __LINE__, "] duplicate handler(msg_id:", msg_id, ")");
#else
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] duplicate handler(msg_id:", msg_id, ")");
#endif
		}

#ifdef _DEBUG
		std::shared_ptr<HandlerCallInfo> handlerCallInfo(new HandlerCallInfo());
		mapHandlerCallInfo_.insert(std::make_pair(msg_id, handlerCallInfo));
#endif
		return true;
	}

	void OnRecvMsg(std::shared_ptr<SESSION_T> session, std::shared_ptr<Packet> packet)
	{
		const unsigned int msg_id = packet->GetID();
		auto itr = mapHandlerFunction_.find(msg_id);
		if(itr == mapHandlerFunction_.end())
		{
			Log::Write(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ", session_key:", session->sessionKey_,",packet_size:", packet->Size(), ", packet_read_cursor:", packet->readCursor_, ")");
			session->OnError(EINVAL);
			return ;
		}

		const HandlerFunction& handler_function = itr->second;
		std::shared_ptr<IHandler> handler = handler_function.factory_->GetHandler(&session->handlerContainer_, msg_id);
		if(NULL == handler)
		{
			Log::Write(GAMNET_ERR, "can't find handler function(msg_id:", msg_id, ", session_key:", session->sessionKey_,",packet_size:", packet->Size(), ", packet_read_cursor:", packet->readCursor_, ")");
			session->OnError(EINVAL);
			return;
		}
#ifdef _DEBUG

		boost::posix_time::ptime tick = boost::posix_time::second_clock::local_time();
		if (mapHandlerCallInfo_.end() != mapHandlerCallInfo_.find(msg_id))
		{
			std::shared_ptr<HandlerCallInfo> callInfo = mapHandlerCallInfo_[msg_id];
			callInfo->beginCount++;
		}
#endif
		try {
			handler_function.function_(handler, session, packet);
		}
		catch (const std::exception& e)
		{
			Log::Write(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
		}
#ifdef _DEBUG
		if (mapHandlerCallInfo_.end() != mapHandlerCallInfo_.find(msg_id))
		{
			std::shared_ptr<HandlerCallInfo> callInfo = mapHandlerCallInfo_[msg_id];
			boost::posix_time::time_duration diff = boost::posix_time::second_clock::local_time() - tick;
			callInfo->elapsedTime += diff.total_milliseconds();
			callInfo->finishCount++;
		}
#endif
	}
};

}}
#endif /* DISPATCHER_H_ */
