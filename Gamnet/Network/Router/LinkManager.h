#ifndef GAMNET_NETWORK_ROUTER_LINKMANAGER_H_
#define GAMNET_NETWORK_ROUTER_LINKMANAGER_H_

#include <list>
#include <set>

#include "Session.h"
#include "../Tcp/LinkManager.h"

namespace Gamnet { namespace Network { namespace Router {

struct LinkManager : public Tcp::LinkManager<Session> {
public :
	Timer heartbeat_timer;
	Address local_address;

	static std::mutex lock;
	static std::function<void(const Address& addr)> onRouterAccept;
	static std::function<void(const Address& addr)> onRouterClose;
public :
	LinkManager();
	virtual ~LinkManager();

	void Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose);
	void Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose);

	void OnAccept(const std::shared_ptr<Network::Link>& link) override;
	void OnConnect(const std::shared_ptr<Network::Link>& link) override;
	void OnClose(const std::shared_ptr<Network::Link>& link, int reason) override;
};

}}}
#endif /* ROUTERIMPL_H_ */
