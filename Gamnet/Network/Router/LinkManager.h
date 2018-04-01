#ifndef GAMNET_NETWORK_ROUTER_LINKMANAGER_H_
#define GAMNET_NETWORK_ROUTER_LINKMANAGER_H_

#include <list>
#include <set>

#include "Session.h"
#include "../Tcp/LinkManager.h"
#include "../Tcp/CastGroup.h"

namespace Gamnet { namespace Network { namespace Router {

struct LinkManager : public Tcp::LinkManager<Session> {
	Timer _heartbeat_timer;
	std::shared_ptr<Tcp::CastGroup> _cast_group;
public :
	Address local_address;
	static std::mutex lock;
	static std::function<void(const Address& addr)> onRouterAccept;
	static std::function<void(const Address& addr)> onRouterClose;
public :
	LinkManager();
	virtual ~LinkManager();

	void Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose);
	void Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose);

	virtual std::shared_ptr<Network::Link> Create() override;
	virtual void OnAccept(const std::shared_ptr<Network::Link>& link) override;
	virtual void OnConnect(const std::shared_ptr<Network::Link>& link) override;
	virtual void OnClose(const std::shared_ptr<Network::Link>& link, int reason) override;
	virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) override;
};

}}}
#endif /* ROUTERIMPL_H_ */
