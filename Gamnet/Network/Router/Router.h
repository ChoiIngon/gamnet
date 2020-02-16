#ifndef GAMNET_NETWORK_ROUTER_H_
#define GAMNET_NETWORK_ROUTER_H_

#include "../Tcp/Tcp.h"
#include "RouterCaster.h"
#include "Dispatcher.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network { namespace Router {
	const Address& GetRouterAddress();

	void Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& accept_callback = [](const Address&){}, const std::function<void(const Address& addr)>& close_callback = [](const Address&) {});
	void Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& connect_callback = [](const Address&) {}, const std::function<void(const Address& addr)>& close_callback = [](const Address&) {});
	
	void ReadXml(const char* xml_path, const std::function<void(const Address& addr)>& connect_callback, const std::function<void(const Address& addr)>& close_callback);

	template <class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher>::GetInstance().RegisterHandler(msg_id, func, factory);
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
	static bool Router_##message_type##_##func = Gamnet::Network::Router::RegisterHandler( \
		message_type::MSG_ID, \
		&class_type::func, \
		new Gamnet::Network::policy<class_type>() \
	)

#endif /* ROUTER_H_ */
