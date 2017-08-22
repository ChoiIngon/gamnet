/*
 * Listener.h
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HTTP_LINKMANAGER_H_
#define GAMNET_NETWORK_HTTP_LINKMANAGER_H_

#include "../LinkManager.h"
#include "Session.h"

namespace Gamnet { namespace Network { namespace Http {

class LinkManager : public Network::LinkManager
{
protected :
	std::mutex lock_;
public :
	LinkManager();
	virtual ~LinkManager();

	virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer);
};

}}}
#endif /* LISTENER_H_ */
