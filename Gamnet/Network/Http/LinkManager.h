/*
 * Listener.h
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HTTP_LINKMANAGER_H_
#define GAMNET_NETWORK_HTTP_LINKMANAGER_H_

#include "../Tcp/LinkManager.h"
#include "Session.h"

namespace Gamnet { namespace Network { namespace Http {

class LinkManager : public Network::LinkManager
{
public :
	LinkManager();
	virtual ~LinkManager();

	virtual void OnAccept(const std::shared_ptr<Link>& link) override;
	virtual void OnClose(const std::shared_ptr<Link>& link, int reason) override;
	virtual void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::shared_ptr<Buffer>& buffer) override;
};

}}}
#endif /* LISTENER_H_ */
