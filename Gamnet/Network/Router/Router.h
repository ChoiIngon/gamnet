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
	
	template <class MSG>
	bool SendMsg(const Address& addr, const MSG& msg)
	{
		std::shared_ptr<Network::Tcp::Packet> inner = Network::Tcp::Packet::Create();
		if (nullptr == inner)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg:", typeid(MSG).name(), ")");
		}

		if (false == inner->Write(msg))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg:", typeid(MSG).name(), ")");
		}
		
		return Singleton<RouterCaster>::GetInstance().SendMsg(addr, inner);
	}

	template <class MSG>
	bool SendMsg(const Address& addr, const MSG& msg, const std::shared_ptr<Tcp::IAsyncResponse>& response)
	{
		if ((unsigned int)ROUTER_CAST_TYPE::MAX <= (unsigned int)addr.cast_type)
		{
			LOG(ERR, "cast_type:", (unsigned int)addr.cast_type, " is undefined cast_type");
			return false;
		}

		std::shared_ptr<Session> session = Singleton<RouterCaster>::GetInstance().FindSession(addr);
		if (nullptr == session)
		{
			LOG(ERR, "can not find route(route_address:", addr.ToString(), ")");
			return false;
		}

		std::shared_ptr<Tcp::Packet> inner = Tcp::Packet::Create();
		if (nullptr == inner)
		{
			LOG(ERR, "fail to create packet instance(msg_id:", (int)MSG::MSG_ID, ")");
			return false;
		}

		inner->msg_seq = response->seq;
		if (false == inner->Write(msg))
		{
			LOG(ERR, "fail to serialize message(msg_id:", (int)MSG::MSG_ID, ")");
			return false;
		}

		MsgRouter_SendMsg_Ntf ntf;
		ntf.msg_seq = 0;
		ntf.buffer.assign(inner->ReadPtr(), inner->Size());

		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if (nullptr == packet)
		{
			LOG(ERR, "fail to create packet instance(msg:MsgRouter_SendMsg_Ntf)");
			return false;
		}

		if (false == packet->Write(ntf))
		{
			LOG(ERR, "fail to serialize message(msg:MsgRouter_SendMsg_Ntf)");
			return false;
		}

		session->AsyncSend(packet, response);
		return true;
	}

	template <class REQ, class ANS>
	bool SendMsg(const Address& addr, const REQ& req, ANS& ans, int timeout)
	{
		std::shared_ptr<Session> session = Singleton<RouterCaster>::GetInstance().FindSession(addr);
		if (nullptr == session)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidRouterAddress, "can not find route(route_address:", addr.ToString(), ", msg_id:", REQ::MSG_ID, ")");
		}

		std::shared_ptr<Gamnet::Network::Tcp::Packet> inner = Gamnet::Network::Tcp::Packet::Create();
		if(nullptr == inner)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", REQ::MSG_ID, ")");
		}
		if(false == inner->Write(req))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", REQ::MSG_ID, ")");
		}
		
		MsgRouter_SendMsg_Ntf ntf;
		std::copy(inner->ReadPtr(), inner->ReadPtr() + inner->Size(), std::back_inserter(ntf.buffer));

		std::shared_ptr<Gamnet::Network::Tcp::Packet> packet = Gamnet::Network::Tcp::Packet::Create();
		if(nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", REQ::MSG_ID, ")");
		}
		if(false == packet->Write(ntf))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", REQ::MSG_ID, ")");
		}
		
		packet = session->SyncSend(packet, timeout);
		if (nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::SendMsgFailError, "fail to send message(msg_id:", REQ::MSG_ID, ")");
		}

		ntf.buffer.clear();
		if(false == Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", ANS::MSG_ID, ")");
		}

		buffer->Clear();
		buffer->Append(ntf.buffer.data(), ntf.buffer.size());
		if (false == Tcp::Packet::Load(ans, buffer))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", ANS::MSG_ID, ")");
		}
		return true;
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
