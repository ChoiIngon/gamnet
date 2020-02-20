#ifndef GAMNET_NETWORK_LINKMANAGER_H_
#define GAMNET_NETWORK_LINKMANAGER_H_

#include <atomic>
#include <mutex>
#include <boost/bind.hpp>

#include "Link.h"
#include "../Library/Json/json.h"

namespace Gamnet { namespace Network {

class LinkManager
{
	
public :
	std::string	name;
public :
	LinkManager();
	virtual ~LinkManager();

	virtual void OnAccept(const std::shared_ptr<Link>& link) {}
	virtual void OnConnect(const std::shared_ptr<Link>& link) {}
	virtual void OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer) {};
	virtual void OnClose(const std::shared_ptr<Link>& link, int reason) {}	
};

}} /* namespace Gamnet */

#endif /* NETWORK_LISTENER_H_ */
