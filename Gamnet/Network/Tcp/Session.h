#ifndef GAMNET_NETWORK_TCP_SESSION_H_
#define GAMNET_NETWORK_TCP_SESSION_H_

#include "../../Library/Json/json.h"
#include "../Session.h"
#include "Packet.h"

namespace Gamnet { namespace Network { namespace Tcp {

class Session : public Network::Session
{
public :
	std::atomic<uint32_t> msg_seq;
	//std::shared_ptr<Packet> recv_packet;

	Session();
	virtual ~Session();
};
}}}
#endif /* NETWORK_SESSION_H_ */
