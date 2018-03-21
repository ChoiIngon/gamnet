#ifndef GAMNET_NETWORK_HTTP_LINKMANAGER_H_
#define GAMNET_NETWORK_HTTP_LINKMANAGER_H_

#include "Link.h"
#include "../Tcp/LinkManager.h"


namespace Gamnet { namespace Network { namespace Http {

class LinkManager : public Network::LinkManager
{
	struct LinkFactory {
		LinkManager* const link_manager;
		LinkFactory(LinkManager* linkManager) : link_manager(linkManager)
		{
		}

		Link* operator() ()
		{
			return new Link(link_manager);
		}
	};
	Pool<Link, std::mutex, typename Network::Link::InitFunctor> link_pool;
public :
	LinkManager();
	virtual ~LinkManager();

	virtual std::shared_ptr<Network::Link> Create() override;
	virtual void OnAccept(const std::shared_ptr<Network::Link>& link) override;
	virtual void OnClose(const std::shared_ptr<Network::Link>& link, int reason) override;
};

}}}
#endif /* LISTENER_H_ */
