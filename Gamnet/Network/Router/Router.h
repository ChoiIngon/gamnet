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
	
	template <class MSG>
	bool SendMsg(const Address& addr, const MSG& msg, std::function<void(const std::shared_ptr<Tcp::Packet>&)> onReceive, std::function<void()> onTimeout, int timeout)
	{
		std::shared_ptr<Session> session = Singleton<RouterCaster>::GetInstance().FindSession(addr);
		if (nullptr == session)
		{
			LOG(ERR, "can not find route(route_address:", addr.ToString(), ")");
			return false;
		}

		std::shared_ptr<Tcp::Packet> buffer = Tcp::Packet::Create();
		if (nullptr == buffer)
		{
			LOG(ERR, "fail to create packet instance(msg_id:", MSG::MSG_ID, ")");
			return false;
		}

		if (false == buffer->Write(msg))
		{
			LOG(ERR, "fail to serialize message(msg_id:", MSG::MSG_ID, ")");
			return false;
		}

		MsgRouter_SendMsg_Ntf ntf;
		ntf.msg_seq = ++session->send_seq;
		ntf.buffer.assign(buffer->ReadPtr(), buffer->Size());

		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if (nullptr == packet)
		{
			LOG(ERR, "fail to create packet instance(msg_id:", MsgRouter_SendMsg_Ntf::MSG_ID, ")");
			return false;
		}

		packet->msg_seq = ++session->timeout_seq;
		if (false == packet->Write(ntf))
		{
			LOG(ERR, "fail to serialize message(msg_id:", MsgRouter_SendMsg_Ntf::MSG_ID, ")");
			return false;
		}

		if ((unsigned int)ROUTER_CAST_TYPE::MAX <= (unsigned int)addr.cast_type)
		{
			LOG(ERR, "cast_type:", (unsigned int)addr.cast_type, " is undefined cast_type");
			return false;
		}

		session->AsyncSend(packet, onReceive, onTimeout, timeout);
		return true;
	}
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

	template <class REQ, class ANS>
	bool SendMsg(const Address& addr, const REQ& req, ANS& ans, int timeout)
	{
		std::shared_ptr<Session> session = Singleton<RouterCaster>::GetInstance().FindSession(addr);
		if (nullptr == session)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidRouterAddress, "can not find route(route_address:", addr.ToString(), ", msg_id:", REQ::MSG_ID, ")");
		}

		std::shared_ptr<Gamnet::Network::Tcp::Packet> buffer = Gamnet::Network::Tcp::Packet::Create();
		if(nullptr == buffer)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", REQ::MSG_ID, ")");
		}
		if(false == buffer->Write(req))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", REQ::MSG_ID, ")");
		}
		
		MsgRouter_SendMsg_Ntf ntf;
		ntf.msg_seq = 0;
		std::copy(buffer->ReadPtr(), buffer->ReadPtr() + buffer->Size(), std::back_inserter(ntf.buffer));

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

		ntf.msg_seq = 0;
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

	template <class MSG>
	bool SendMsg(const std::shared_ptr<Session>& session, const MSG& msg)
	{
		std::shared_ptr<Tcp::Packet> buffer = Tcp::Packet::Create();
		if(nullptr == buffer)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", MSG::MSG_ID, ")");
		}

		if(false == buffer->Write(msg))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", MSG::MSG_ID, ")");
		}

		MsgRouter_SendMsg_Ntf ntf;
		ntf.msg_seq = session->send_seq;
		std::copy(buffer->ReadPtr(), buffer->ReadPtr() + buffer->Size(), std::back_inserter(ntf.buffer));

		std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
		if(nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", MSG::MSG_ID, ")");
		}

		packet->msg_seq = session->send_seq;
		if(false == packet->Write(ntf))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", MSG::MSG_ID, ")");
		}

		session->Network::Session::AsyncSend(packet);
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
