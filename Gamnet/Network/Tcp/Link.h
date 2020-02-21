#ifndef GAMNET_NETWORK_TCP_LINK_H_
#define GAMNET_NETWORK_TCP_LINK_H_

#include "Packet.h"
#include "Session.h"
#include "../Link.h"

namespace Gamnet { namespace Network { namespace Tcp {
	class Link : public Network::Link 
	{
	public :
		Link(Network::LinkManager* linkManager);
		virtual ~Link();

	private:
		std::shared_ptr<Packet> recv_packet;
		int64_t expire_time;
	public :
		Network::LinkManager* const link_manager;
		std::shared_ptr<Session>	session;

		virtual bool Init() override;
		virtual void Clear() override;

		void SetKeepAlive(int sec);
	protected :
		virtual void OnAccept() override;
		virtual void OnConnect() override;
		virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;
		virtual void OnClose(int reason) override;
	};
}}}

#endif
