#include "Router.h"
#include "../../Library/Singleton.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Network { namespace Router {

void Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose)
{
	Singleton<LinkManager>::GetInstance().Listen(service_name, port, onAccept, onClose);
	LOG(INF, "Gamnet::Router listener start(port:", port, ", "
			"router_address:",
				Singleton<LinkManager>::GetInstance().local_address.service_name, ":",
				(int)Singleton<LinkManager>::GetInstance().local_address.cast_type, ":",
				Singleton<LinkManager>::GetInstance().local_address.id, ", ",
			"ip:", Network::Tcp::GetLocalAddress().to_string(), ")");
}

void Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose)
{
	Singleton<LinkManager>::GetInstance().Connect(host, port, timeout, onConnect, onClose);
}

}}}
