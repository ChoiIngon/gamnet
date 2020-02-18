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
	std::mutex lock;
	std::map<uint32_t, std::shared_ptr<Network::Link>> links;
	Timer expire_timer;
public :
	std::string	name;
	int	expire_time; // zero means infinity
public :
	LinkManager();
	virtual ~LinkManager();

	virtual void OnAccept(const std::shared_ptr<Link>& link) {}
	virtual void OnConnect(const std::shared_ptr<Link>& link) {}
	virtual void OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer) {};
	virtual void OnClose(const std::shared_ptr<Link>& link, int reason) {}	

	bool Add(const std::shared_ptr<Link>& link);
	void Remove(uint32_t linkKey);
	size_t Size();

protected :
	void ActivateIdleLinkTerminator(int timeout);
private :
	void OnTimerExpire();
};

}} /* namespace Gamnet */

#endif /* NETWORK_LISTENER_H_ */
