/*
 * RouterImpl.h
 *
 *  Created on: Jun 14, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_ROUTER_LISTENER_H_
#define GAMNET_ROUTER_LISTENER_H_

#include <list>
#include <set>

#include "Session.h"
#include "../Network/Network.h"
namespace Gamnet { namespace Router {

struct RouterListener : Network::Listener<Router::Session> {
public :
	Address localAddr_;
	static std::mutex lock;
	static std::function<void(const Address& addr)> onRouterAccept;
	static std::function<void(const Address& addr)> onRouterClose;
public :
	RouterListener();
	virtual ~RouterListener();

	void Init(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose);
	bool Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose);
};

}};
#endif /* ROUTERIMPL_H_ */
