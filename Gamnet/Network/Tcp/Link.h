#ifndef GAMNET_NETWORK_TCP_LINK_H_
#define GAMNET_NETWORK_TCP_LINK_H_

#include "Packet.h"
#include "../Link.h"

namespace Gamnet { namespace Network { namespace Tcp {
	class Link : public Network::Link {
		std::shared_ptr<Packet> recv_packet;
	public :
		Link(Network::LinkManager* linkManager);
		virtual ~Link();

		Network::LinkManager* const link_manager;

		virtual bool Init() override;
		virtual void Clear() override;

		virtual void OnAccept() override;

	protected :
		virtual void OnConnect() override;
		virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;
		virtual void OnClose(int reason) override;
	};
}}}

#endif
