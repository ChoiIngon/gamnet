#ifndef GAMNET_NETWORK_HTTP_LINK_H_
#define GAMNET_NETWORK_HTTP_LINK_H_

#include "../Link.h"

namespace Gamnet { namespace Network { namespace Http {
	class Network::LinkManager;
	class Link : public Network::Link {
	public :
		Link(Network::LinkManager* linkManager);
		virtual ~Link();

		virtual bool Init();
		std::shared_ptr<Buffer> recv_buffer;
		
		virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;
	};
}}}
#endif
