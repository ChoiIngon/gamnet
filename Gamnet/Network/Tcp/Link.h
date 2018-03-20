#ifndef GAMNET_NETWORK_TCP_LINK_H_
#define GAMNET_NETWORK_TCP_LINK_H_

#include "Packet.h"
#include "../Link.h"


namespace Gamnet { namespace Network { namespace Tcp {

	class Link : public Network::Link {
	public :
		struct Init
		{
			Link* operator() (Link* link)
			{
				Network::Link::Init init;
				init(link);
				link->recv_packet = Packet::Create();
				return link;
			}
		};

		std::shared_ptr<Packet> recv_packet;
		virtual void OnRecvMsg() override;
	};

}}}

#endif