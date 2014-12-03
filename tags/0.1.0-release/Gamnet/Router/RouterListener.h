/*
 * RouterImpl.h
 *
 *  Created on: Jun 14, 2013
 *      Author: kukuta
 */

#ifndef ROUTERIMPL_H_
#define ROUTERIMPL_H_

#include <list>
#include <set>

#include "Session.h"
#include "../Network/Network.h"
namespace Gamnet { namespace Router {
struct RouterListener : Network::Listener<Router::Session> {
public :

	Address localAddr_;
public :
	RouterListener();
	virtual ~RouterListener();

	void Init(const char* service_name, int port=20001);
	bool Connect(const char* host, int port, int timeout);
};

}};
#endif /* ROUTERIMPL_H_ */
