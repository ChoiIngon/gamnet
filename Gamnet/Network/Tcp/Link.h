#ifndef GAMNET_NETWORK_TCP_LINK_H_
#define GAMNET_NETWORK_TCP_LINK_H_

#include "Packet.h"
#include "../Link.h"

namespace Gamnet { namespace Network { namespace Tcp {
	class Link : public Network::Link {
	public :
		enum MSG_ID {
			MsgID_CliSvr_Connect_Req = 1,
			MsgID_SvrCli_Connect_Ans = 1,
			MsgID_CliSvr_Reconnect_Req = 2,
			MsgID_SvrCli_Reconnect_Ans = 2,
			MsgID_CliSvr_HeartBeat_Ntf = 3,
			MsgID_SvrCli_HeartBeat_Ntf = 3,
			MsgID_SvrCli_Kickout_Ntf = 4,
			MsgID_CliSvr_Close_Ntf = 5,
			MsgID_Max = 6
		};

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
	protected :
		void Send_HeartBeat_Ntf();
	};
}}}

#endif
