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
#include "../../Library/Time/Timer.h"
#include "../Handler.h"
#include "../HandlerFactory.h"
#include "Packet.h"
#include "SystemMessageHandler.h"

namespace Gamnet {	namespace Network { namespace Tcp {
	class IHandlerFunctor
	{
	public:
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
		IHandlerFunctor(const std::string& name, IHandlerFactory* factory) : name(name), factory(factory) {}
		virtual void OnReceive(const std::shared_ptr<Session>&, const std::shared_ptr<Packet>&) = 0;

		Statistics statistics;
		const std::string name;
	protected:
		IHandlerFactory* const factory;
	};

	template <class SessionType, class MsgType>
	class HandlerFunctor : public IHandlerFunctor
	{
	public:
		typedef void(IHandler::*FunctionType)(const std::shared_ptr<SessionType>&, const MsgType&);

		template<class FactoryType>
		HandlerFunctor(const std::string& name, FactoryType* factory, FunctionType function) : IHandlerFunctor(name, factory), function(function) {}

		virtual void OnReceive(const std::shared_ptr<Session>& session, const std::shared_ptr<Packet>& packet) override
		{
			std::shared_ptr<IHandler> handler = factory->GetHandler();
			if (nullptr == handler)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidHandlerError, "can't create message handler instance(name:", name, ", msg_id:", MsgType::MSG_ID, ")");
			}

			MsgType msg;
			if (false == Gamnet::Network::Tcp::Packet::Load(msg, packet))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail(name:", name, ", msg_id:", MsgType::MSG_ID, ")");
			}

			function(handler, std::static_pointer_cast<SessionType>(session), msg);
		}
	private:
		std::function<void(const std::shared_ptr<IHandler>&, const std::shared_ptr<SessionType>&, const MsgType&)> function;
	};

	template <class SessionType>
	class HandlerFunctor<SessionType, std::shared_ptr<Packet>> : public IHandlerFunctor
	{
	public:
		typedef std::shared_ptr<Packet> MsgType;
		typedef void(IHandler::* FunctionType)(const std::shared_ptr<SessionType>&, const std::shared_ptr<Packet>&);

		template<class FactoryType>
		HandlerFunctor(const std::string& name, FactoryType* factory, FunctionType function) : IHandlerFunctor(name, factory), function(function) {}

		virtual void OnReceive(const std::shared_ptr<Session>& session, const std::shared_ptr<Packet>& packet) override
		{
			std::shared_ptr<IHandler> handler = factory->GetHandler();
			if (nullptr == handler)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidHandlerError, "can't create message handler instance(name:", name, ")");
			}

			function(handler, std::static_pointer_cast<SessionType>(session), packet);
		}
	private:
		std::function<void(const std::shared_ptr<IHandler>&, const std::shared_ptr<SessionType>&, const std::shared_ptr<Packet>&)> function;
	};

	template <class SESSION_T>
	class Dispatcher
	{
		GAMNET_WHERE(SESSION_T, Session);
	public:
		Dispatcher()
		{
			typedef HandlerFunctor<SESSION_T, std::shared_ptr<Packet>> PacketHandlerFunctorType;
			IHandlerFactory* handlerFactory = new HandlerStatic<SystemMessageHandler<SESSION_T>>();
			BindHandler(MSG_ID::MsgID_CliSvr_Connect_Req,	std::make_shared<PacketHandlerFunctorType>(
				"MsgID_CliSvr_Connect_Req",		handlerFactory, static_cast<typename PacketHandlerFunctorType::FunctionType>(&SystemMessageHandler<SESSION_T>::Recv_Connect_Req)
			));
			BindHandler(MSG_ID::MsgID_CliSvr_Reconnect_Req,	std::make_shared<PacketHandlerFunctorType>(
				"MsgID_CliSvr_Reconnect_Req",	handlerFactory, static_cast<typename PacketHandlerFunctorType::FunctionType>(&SystemMessageHandler<SESSION_T>::Recv_Reconnect_Req)
			));
			BindHandler(MSG_ID::MsgID_CliSvr_Close_Req,		std::make_shared<PacketHandlerFunctorType>(
				"MsgID_CliSvr_Close_Req",		handlerFactory, static_cast<typename PacketHandlerFunctorType::FunctionType>(&SystemMessageHandler<SESSION_T>::Recv_Close_Req)
			));
			BindHandler(MSG_ID::MsgID_CliSvr_HeartBeat_Req,	std::make_shared<PacketHandlerFunctorType>(
				"MsgID_CliSvr_HeartBeat_Req",	handlerFactory, static_cast<typename PacketHandlerFunctorType::FunctionType>(&SystemMessageHandler<SESSION_T>::Recv_HeartBeat_Req)
			));
			BindHandler(MSG_ID::MsgID_CliSvr_ReliableAck_Ntf, std::make_shared<PacketHandlerFunctorType>(
				"MsgID_CliSvr_ReliableAck_Ntf", handlerFactory,	static_cast<typename PacketHandlerFunctorType::FunctionType>(&SystemMessageHandler<SESSION_T>::Recv_ReliableAck_Ntf)
			));
		}
		~Dispatcher() {}

		bool BindHandler(uint32_t msg_id, const std::shared_ptr<IHandlerFunctor>& handlerFunctor)
		{
			if (false == handler_functors.insert(std::make_pair(msg_id, handlerFunctor)).second)
			{
#ifdef _WIN32
				MessageBoxA(nullptr, Format("duplicate handler(msg_id:", msg_id, ", name : ", handlerFunctor->name, ")").c_str(), "Duplicate Handler Error", MB_ICONWARNING);
#endif
				throw Exception(0, "[", __FILE__, ":", __func__, "@", __LINE__, "] duplicate handler(msg_id:", msg_id, ", name:", handlerFunctor->name, ")");
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

			const std::shared_ptr<IHandlerFunctor>& handlerFunctor = itr->second;
#ifdef _DEBUG
			Time::ElapseTimer elapseTimer;
			IHandlerFunctor::Statistics& statistics = handlerFunctor->statistics;
			statistics.begin_count++;
#endif
			handlerFunctor->OnReceive(session, packet);
			
			if (true == packet->reliable)
			{
				session->recv_seq = packet->msg_seq;
			}
#ifdef _DEBUG
			int64_t elapsedTime = elapseTimer.Count();
			statistics.max_time = std::max(statistics.max_time, elapsedTime);
			statistics.total_time += elapsedTime;
			statistics.finish_count++;
#endif
		}

	private :
		std::map<uint32_t, std::shared_ptr<IHandlerFunctor>> handler_functors;
	};
}}}
#endif /* DISPATCHER_H_ */
