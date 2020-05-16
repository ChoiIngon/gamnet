/*
 * Dispatcher.h
 *
 *  Created on: Aug 7, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_TCP_DISPATCHER_H_
#define GAMNET_NETWORK_TCP_DISPATCHER_H_
#include <algorithm>
#include <map>
#include <atomic>

#include "../../Log/Log.h"
#include "../Handler.h"
#include "../HandlerFactory.h"
#include "Packet.h"
#include "SystemMessageHandler.h"

namespace Gamnet {	namespace Network { namespace Tcp {

	template <class SESSION_T>
	class Dispatcher
	{
		GAMNET_WHERE(SESSION_T, Session);

		typedef void(IHandler::*function_type)(const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Packet>&);
#ifdef _DEBUG
		struct Statistics
		{
			std::atomic<int64_t> begin_count;
			std::atomic<int64_t> finish_count;
			std::atomic<int64_t> total_time;
			int64_t max_time;

			Statistics() : begin_count(0), finish_count(0), total_time(0), max_time(0)
			{
			}
		};
#endif
		struct HandlerFunctor
		{
			template<class FACTORY, class FUNCTION>
#ifdef _WIN32
			HandlerFunctor(const std::string& name, FACTORY* factory, FUNCTION function) : name(name), factory(factory), function(std::mem_fn(function)) {}
#else
			HandlerFunctor(const std::string& name, FACTORY* factory, FUNCTION function) : name(name), factory(factory), function(function) {}
#endif
			const std::string& name;
			IHandlerFactory* factory;
			std::function<void(const std::shared_ptr<IHandler>&, const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Packet>&)> function;
#ifdef _DEBUG
			Statistics* statistics;
#endif
		};
		std::map<uint32_t, HandlerFunctor> handler_functors;
	public:
		Dispatcher()
		{
			BindHandler(MSG_ID::MsgID_CliSvr_Connect_Req, "MsgID_CliSvr_Connect_Req", &SystemMessageHandler<SESSION_T>::Recv_Connect_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
			BindHandler(MSG_ID::MsgID_CliSvr_Reconnect_Req, "MsgID_CliSvr_Reconnect_Req", &SystemMessageHandler<SESSION_T>::Recv_Reconnect_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
			BindHandler(MSG_ID::MsgID_CliSvr_Close_Req, "MsgID_CliSvr_Close_Req", &SystemMessageHandler<SESSION_T>::Recv_Close_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
			BindHandler(MSG_ID::MsgID_CliSvr_HeartBeat_Req, "MsgID_CliSvr_HeartBeat_Req", &SystemMessageHandler<SESSION_T>::Recv_HeartBeat_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
			BindHandler(MSG_ID::MsgID_CliSvr_ReliableAck_Ntf, "MsgID_CliSvr_ReliableAck_Ntf", &SystemMessageHandler<SESSION_T>::Recv_ReliableAck_Ntf, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
		}
		~Dispatcher() {}

		template <class FUNC, class FACTORY>
		bool BindHandler(uint32_t msg_id, const char* name, FUNC func, FACTORY factory)
		{
			HandlerFunctor handlerFunctor(name, factory, static_cast<function_type>(func));
#ifdef _DEBUG
			handlerFunctor.statistics = new Statistics();
#endif

			if (false == handler_functors.insert(std::make_pair(msg_id, handlerFunctor)).second)
			{
#ifdef _WIN32
				MessageBoxA(nullptr, Format("duplicate handler(msg_id:", msg_id, ", name : ", name, ")").c_str(), "Duplicate Handler Error", MB_ICONWARNING);
#endif
				throw Exception(0, "[", __FILE__, ":", __func__, "@", __LINE__, "] duplicate handler(msg_id:", msg_id, ", name:", name, ")");
			}
			return true;
		}

		void OnReceive(const std::shared_ptr<SESSION_T>& session, const std::shared_ptr<Packet>& packet)
		{
			if (true == packet->reliable && packet->msg_seq <= session->recv_seq)
			{
				LOG(WRN, "discard message(msg_id:", packet->msg_id, ", received msg_seq:", packet->msg_seq, ", expected msg_seq:", session->recv_seq + 1, ")");
				return;
			}

			auto itr = handler_functors.find(packet->msg_id);
			if (handler_functors.end() == itr)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidHandlerError, "can't find handler function(msg_id:", packet->msg_id, ")");
			}

			const HandlerFunctor& handlerFunctor = itr->second;
			std::shared_ptr<IHandler> handler = handlerFunctor.factory->GetHandler(&session->handler_container, packet->msg_id);
			if (nullptr == handler)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidHandlerError, "can't find handler function(msg_id:", packet->msg_id, ")");
			}
#ifdef _DEBUG
			Time::ElapseTimer elapseTimer;
			Statistics* statistics = handlerFunctor.statistics;
			statistics->begin_count++;
#endif
			handlerFunctor.function(handler, session, packet);

			if (true == packet->reliable)
			{
				session->recv_seq = packet->msg_seq;
			}
#ifdef _DEBUG
			int64_t elapsedTime = elapseTimer.Count();
			statistics->max_time = std::max(statistics->max_time, elapsedTime);
			statistics->total_time += elapsedTime;
			statistics->finish_count++;
#endif
		}
	};
}}}
#endif /* DISPATCHER_H_ */
