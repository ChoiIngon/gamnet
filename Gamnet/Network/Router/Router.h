#ifndef GAMNET_NETWORK_ROUTER_H_
#define GAMNET_NETWORK_ROUTER_H_

#include "../Tcp/Tcp.h"
#include "RouterCaster.h"
#include "Dispatcher.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network { namespace Router {

void Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose);
void Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose);
template <class FUNC, class FACTORY>
bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
{
	return Singleton<Dispatcher>::GetInstance().RegisterHandler(msg_id, func, factory);
}

template <class REQ>
bool SendMsg(std::shared_ptr<Network::Session> session, const Address& addr, const REQ& msg)
{
	std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
	if(NULL == packet)
	{
		return false;
	}
	if(false == packet->Write(msg))
	{
		return false;
	}

	if(false == Singleton<RouterCaster>::GetInstance().SendMsg(session, addr, packet->ReadPtr(), packet->Size()))
	{
		return false;
	}
	return true;
}

template <class MSG>
bool SendMsg(const Address& addr, const MSG& msg)
{
	return SendMsg(NULL, addr, msg);
}

}}}

#define GAMNET_BIND_ROUTER_HANDLER(message_type, class_type, func, policy) \
	static bool Router_##class_type##_##func = Gamnet::Router::RegisterHandler( \
		message_type::MSG_ID, \
		&class_type::func, \
		new Gamnet::Network::policy<class_type>() \
	)

#endif /* ROUTER_H_ */
