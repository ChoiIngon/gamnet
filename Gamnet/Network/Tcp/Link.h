#ifndef GAMNET_NETWORK_TCP_LINK_H_
#define GAMNET_NETWORK_TCP_LINK_H_

#include "Packet.h"
#include "../Link.h"

namespace Gamnet { namespace Network { namespace Tcp {
	class Link : public Network::Link {
	public :
		enum {
			RELIABLE_PACKET_QUEUE_SIZE = 50
		};
	public :
		Link(Network::LinkManager* linkManager);
		virtual ~Link();

		virtual bool Init() override;
		
		uint32_t recv_seq;
		std::shared_ptr<Packet> recv_packet;
	protected :
		virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;
		virtual void OnSend(const boost::system::error_code& ec, std::size_t transferredBytes) override;
	};
}}}

#endif
