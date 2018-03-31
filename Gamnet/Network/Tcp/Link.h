#ifndef GAMNET_NETWORK_TCP_LINK_H_
#define GAMNET_NETWORK_TCP_LINK_H_

#include "Packet.h"
#include "../Link.h"

namespace Gamnet { namespace Network { namespace Tcp {
	class Link : public Network::Link {
	public :
		Link(Network::LinkManager* linkManager);
		virtual ~Link();

		virtual bool Init() override;
		
		uint32_t msg_seq;
		std::shared_ptr<Packet> recv_packet;
		
		virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;
	};
}}}

#endif
