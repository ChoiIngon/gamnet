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
	bool SendMsg(const std::shared_ptr<Network::Tcp::Session>& session, const Address& addr, const MSG& msg)
	{
		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if(nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", MSG::MSG_ID, ")");
		}

		packet->msg_seq = 0;
		packet->reliable = false;

		if(false == packet->Write(msg))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", MSG::MSG_ID, ")");
		}

		return Singleton<RouterCaster>::GetInstance().SendMsg(session, addr, packet->ReadPtr(), (int)packet->Size());
	}

	template <class MSG>
	bool SendMsg(const Address& addr, const MSG& msg)
	{
		return SendMsg(nullptr, addr, msg);
	}
}}}

#define GAMNET_BIND_ROUTER_HANDLER(message_type, class_type, func, policy) \
	static bool Router_##message_type##_##func = Gamnet::Network::Router::BindHandler( \
		message_type::MSG_ID, \
		&class_type::func, \
		new Gamnet::Network::policy<class_type>() \
	)

#endif /* ROUTER_H_ */
