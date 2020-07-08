#ifndef GAMNET_NETWORK_ROUTER_H_
#define GAMNET_NETWORK_ROUTER_H_

#include "RouterCaster.h"
#include "Dispatcher.h"


namespace Gamnet { namespace Network { namespace Router 
{
	void ReadXml(const std::string& path, const std::function<void(const Address& addr)>& connectHandler = [](const Address&) {}, const std::function<void(const Address& addr)>& closeHandler = [](const Address&) {});

	const Address& GetRouterAddress();
	void Listen(const std::string& serviceName, int port, const std::function<void(const Address& addr)>& acceptHandler = [](const Address&){}, const std::function<void(const Address& addr)>& closeHandler = [](const Address&) {});
	void Connect(const std::string& host, int port, int timeout);

	template <class FUNC, class FACTORY>
	bool BindHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher>::GetInstance().BindHandler(msg_id, func, factory);
	}
	/*
	class SendResult
	{
		bool send_result;
		uint64_t msg_seq;
	public:
		SendResult(bool sendResult, uint64_t msgSEQ);
		template<class MSG>
		bool WaitResponse(const std::shared_ptr<Network::Tcp::Session>& session, std::function<void()> onTimeout, int seconds = 5)
		{
			if(false == send_result)
			{
				return false;
			}

			if(nullptr == session->current_handler)
			{
				LOG(ERR, "current message handler is null");
				return false;
			}

			session->handler_container.Register(msg_seq, session->current_handler);

			std::shared_ptr<Dispatcher::WaitResponse> waitResponse = std::make_shared<Dispatcher::WaitResponse>();
			waitResponse->expire_time = time(nullptr) + seconds;
			waitResponse->on_timeout = onTimeout;
			waitResponse->session = session;
			Singleton<Dispatcher>::GetInstance().RegisterWaitResponse(MSG::MSG_ID, msg_seq, waitResponse);
			return true;
		}

		operator bool();
	};
	*/	
	template <class REQ, class ANS>
	bool SendMsg(const Address& addr, const REQ& req, std::shared_ptr<IHandler> handler, int seconds, std::function<void()> onTimeout)
	{
		std::shared_ptr<Session> session = Singleton<RouterCaster>::GetInstance().FindSession(addr);
		if(nullptr == session)
		{
			LOG(ERR, "can not find route(route_address:", addr.ToString(), ")");
			return false;
		}

		std::shared_ptr<Network::Tcp::Packet> buffer = Network::Tcp::Packet::Create();
		if(nullptr == buffer)
		{
			LOG(ERR, "fail to create packet instance(msg_id:", REQ::MSG_ID, ")");
			return false;
		}

		if(false == buffer->Write(req))
		{
			LOG(ERR, "fail to serialize message(msg_id:", REQ::MSG_ID, ")");
			return false;
		}

		MsgRouter_SendMsg_Ntf ntf;
		ntf.msg_seq = ++session->send_seq;
		ntf.buffer.assign(buffer->ReadPtr(), buffer->Size());

		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if(nullptr == packet)
		{
			LOG(ERR, "fail to create packet instance(msg_id:", MsgRouter_SendMsg_Ntf::MSG_ID, ")");
			return false;
		}

		if(false == packet->Write(ntf))
		{
			LOG(ERR, "fail to serialize message(msg_id:", MsgRouter_SendMsg_Ntf::MSG_ID, ")");
			return false;
		}

		if((unsigned int)ROUTER_CAST_TYPE::MAX <= (unsigned int)addr.cast_type)
		{
			LOG(ERR, "cast_type:", (unsigned int)addr.cast_type, " is undefined cast_type");
			return false;
		}

		session->handler_container.Register(++session->send_seq, handler);
		session->AsyncSend(packet);
		return true;
	}

	template <class MSG>
	bool SendMsg(Address addr, const MSG& msg)
	{
		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if(nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", MSG::MSG_ID, ")");
		}

		if(false == packet->Write(msg))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", MSG::MSG_ID, ")");
		}

		return Singleton<RouterCaster>::GetInstance().SendMsg(addr, packet);
	}

	template <class MSG>
	bool SendMsg(const std::shared_ptr<Session>& session, const MSG& msg)
	{
		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if(nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", MSG::MSG_ID, ")");
		}

		if(false == packet->Write(msg))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", MSG::MSG_ID, ")");
		}

		session->AsyncSend(packet);
		return true;
	}
}}}

#define GAMNET_BIND_ROUTER_HANDLER(message_type, class_type, func, policy) \
	static bool Router_##message_type##_##func = Gamnet::Network::Router::BindHandler( \
		message_type::MSG_ID, \
		&class_type::func, \
		new Gamnet::Network::policy<class_type>() \
	)

#endif /* ROUTER_H_ */
