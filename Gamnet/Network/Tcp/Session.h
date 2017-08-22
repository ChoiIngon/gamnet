#ifndef GAMNET_NETWORK_TCP_SESSION_H_
#define GAMNET_NETWORK_TCP_SESSION_H_

#include "Packet.h"
#include "../Session.h"

namespace Gamnet { namespace Network { namespace Tcp {

class Session : public Network::Session
{
public :
	struct Init
	{
		template <class T>
		T* operator() (T* session)
		{
			session->msg_seq = 0;
			session->recv_packet = Packet::Create();
			Network::Session::Init init;
			return init(session);
		}
	};
	uint32_t msg_seq;
	std::shared_ptr<Packet> recv_packet;

	Session();
	virtual ~Session();

	virtual void OnAccept() = 0;
	virtual void OnClose(int reason) = 0;

	int Send(const std::shared_ptr<Packet>& packet);
	int Send(const char* buf, int len);
};
}}}
#endif /* NETWORK_SESSION_H_ */
