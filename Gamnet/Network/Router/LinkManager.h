#ifndef GAMNET_NETWORK_ROUTER_LINKMANAGER_H_
#define GAMNET_NETWORK_ROUTER_LINKMANAGER_H_

#include <list>
#include <set>

#include "Session.h"
#include "../Tcp/LinkManager.h"

namespace Gamnet { namespace Network { namespace Router {

struct LinkManager : public Tcp::LinkManager<Router::Session> {
public :
	Address localAddr_;

	static std::mutex lock;
	static std::function<void(const Address& addr)> onRouterAccept;
	static std::function<void(const Address& addr)> onRouterClose;
public :
	LinkManager();
	virtual ~LinkManager();

	void Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose);
	void Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose);

	void OnConnect(const std::shared_ptr<Link>& link);
	void OnClose(const std::shared_ptr<Link>& link, int reason);
	void OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer);
};

}}}
#endif /* ROUTERIMPL_H_ */
