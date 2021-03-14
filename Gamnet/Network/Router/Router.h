#ifndef GAMNET_NETWORK_ROUTER_H_
#define GAMNET_NETWORK_ROUTER_H_

#include "../../Library/Singleton.h"
#include "RouterCaster.h"
#include "Dispatcher.h"


namespace Gamnet { namespace Network { namespace Router 
{
	void ReadXml(const std::string& path, const std::function<void(const Address& addr)>& connectHandler = [](const Address&) {}, const std::function<void(const Address& addr)>& closeHandler = [](const Address&) {});

	const Address& GetRouterAddress();
	void Listen(const std::string& serviceName, int port, const std::function<void(const Address& addr)>& acceptHandler = [](const Address&){}, const std::function<void(const Address& addr)>& closeHandler = [](const Address&) {});
	void Listen(const Address& routerAddress, int port, const std::function<void(const Address& addr)>& acceptHandler = [](const Address&) {}, const std::function<void(const Address& addr)>& closeHandler = [](const Address&) {});
	void Connect(const std::string& host, int port, int timeout);

	template <class MsgType, class FunctionType, class FactoryType>
	bool BindHandler(const std::string& name, FunctionType function, FactoryType factory)
	{
		typedef HandlerFunctor<MsgType> HandlerFunctorType;
		std::shared_ptr<IHandlerFunctor> handlerFunctor = std::make_shared<HandlerFunctorType>(name, factory, static_cast<typename HandlerFunctorType::FunctionType>(function));
		return Singleton<Dispatcher>::GetInstance().BindHandler(MsgType::MSG_ID, handlerFunctor);
	}
	
	/**
		@return : 메시지 보낸 카운트
			UNI_CAST : 0 = 보내기 실패, 1 = 전송 메시지 1개, 1 초과 = 오류
			ANY_CAST : 0 = 보내기 실패, 1 = 전송 메시지 1개, 1 초과 = 오류
			MULTI_CAST : 0 = 보내기 실패, 대상 서버 갯수. 멀티 캐스트 이후 총 몇대의 서버로 부터 응답을 받아야 정상 상황인지 판단 할 때 사용
	*/
	template <class MSG>
	int SendMsg(const Address& addr, const MSG& msg)
	{
		std::shared_ptr<Network::Tcp::Packet> inner = Network::Tcp::Packet::Create();
		if (nullptr == inner)
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to create packet instance(msg:", typeid(MSG).name(), ")");
			return 0;
		}

		if (false == inner->Write(msg))
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to serialize message(msg:", typeid(MSG).name(), ")");
			return 0;
		}
		
		return Singleton<RouterCaster>::GetInstance().SendMsg(addr, inner);
	}

	template <class MSG>
	int SendMsg(const Address& addr, const MSG& msg, const std::shared_ptr<IAsyncResponse>& response)
	{
		if ((int)Address::CAST_TYPE::MAX <= (int)addr.cast_type)
		{
			LOG(ERR, "[Gamnet::Network::Router] invalid router cast type(cast_type:", (unsigned int)addr.cast_type, ")");
			return 0;
		}

		std::shared_ptr<Session> session = Singleton<RouterCaster>::GetInstance().FindSession(addr);
		if (nullptr == session)
		{
			LOG(ERR, "[Gamnet::Network::Router] can not find route(route_address:", addr.ToString(), ", msg:", typeid(MSG).name(), ")");
			return 0;
		}

		std::shared_ptr<Tcp::Packet> inner = Tcp::Packet::Create();
		if (nullptr == inner)
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to create packet instance(msg:", typeid(MSG).name(), ")");
			return 0;
		}

		inner->msg_seq = response->seq;
		if (false == inner->Write(msg))
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to serialize message(msg:", typeid(MSG).name(), ")");
			return 0;
		}

		MsgRouter_SendMsg_Ntf ntf;
		ntf.msg_seq = addr.msg_seq;
		ntf.buffer.assign(inner->ReadPtr(), inner->Size());

		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if (nullptr == packet)
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to create packet instance(msg:MsgRouter_SendMsg_Ntf)");
			return 0;
		}

		if (false == packet->Write(ntf))
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to serialize message(msg:MsgRouter_SendMsg_Ntf)");
			return 0;
		}

		session->AsyncSend(packet, response);
		return 0;
	}

	template <class REQ, class ANS>
	int SendMsg(const Address& addr, const REQ& req, ANS& ans, int timeout)
	{
		std::shared_ptr<Session> session = Singleton<RouterCaster>::GetInstance().FindSession(addr);
		if (nullptr == session)
		{
			LOG(ERR, "[Gamnet::Network::Router] can not find route(route_address:", addr.ToString(), ", msg:", typeid(REQ).name(), ")");
			return 0;
		}

		std::shared_ptr<Gamnet::Network::Tcp::Packet> buffer = Gamnet::Network::Tcp::Packet::Create();
		if(nullptr == buffer)
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to create packet instance(msg:", typeid(REQ).name(), ")");
			return 0;
		}
		if(false == buffer->Write(req))
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to serialize message(msg:", typeid(REQ).name(), ")");
			return 0;
		}
		
		MsgRouter_SendMsg_Ntf ntf;
		std::copy(buffer->ReadPtr(), buffer->ReadPtr() + buffer->Size(), std::back_inserter(ntf.buffer));

		std::shared_ptr<Gamnet::Network::Tcp::Packet> packet = Gamnet::Network::Tcp::Packet::Create();
		if(nullptr == packet)
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to create packet instance(msg:", typeid(REQ).name(), ")");
			return 0;
		}
		if(false == packet->Write(ntf))
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to serialize message(msg:", typeid(REQ).name(), ")");
			return 0;
		}
		
		packet = session->SyncSend(packet, timeout);
		if (nullptr == packet)
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to send message(msg:", typeid(REQ).name(), ")");
			return 0;
		}

		ntf.buffer.clear();
		if(false == Tcp::Packet::Load(ntf, packet))
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to serialize message(msg:", typeid(ANS).name(), ")");
			return 0;
		}

		buffer->Clear();
		buffer->Append(ntf.buffer.data(), ntf.buffer.size());
		if (false == Tcp::Packet::Load(ans, buffer))
		{
			LOG(ERR, "[Gamnet::Network::Router] fail to serialize message(msg:", typeid(ANS).name(), ")");
		}
		return 1;
	}
}}}

#define GAMNET_BIND_ROUTER_HANDLER(message_type, class_type, func, policy) \
	static bool TOKEN_PASTE2(Router_##class_type##_##func, __LINE__) = Gamnet::Network::Router::BindHandler<message_type>( \
		#message_type, \
		&class_type::func, \
		new Gamnet::Network::policy<class_type>() \
	)
#define GAMNET_BIND_RESPONSE_HANDLER(message_type, class_type, func_type, ...) \
	std::make_shared<Gamnet::Network::Router::ResponseHandler<message_type>>(std::bind(&class_type::func_type, std::static_pointer_cast<class_type>(shared_from_this()), ##__VA_ARGS__, std::placeholders::_1))
#endif /* ROUTER_H_ */
