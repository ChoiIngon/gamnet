#include "Router.h"
#include "../../Library/Singleton.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Network { namespace Router {

void Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose)
{
	Singleton<LinkManager>::GetInstance().Listen(service_name, port, onAccept, onClose);
	LOG(GAMNET_INF, "Gamnet router start(service_name:", service_name, ", local ip:", Network::Tcp::GetLocalAddress().to_string(), ", port:", port, ", id:", Singleton<LinkManager>::GetInstance().localAddr_.id, ")");
}

void Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose)
{
	//Singleton<LinkManager>::GetInstance().Connect(host, port, timeout, onConnect, onClose);
}

}}}
