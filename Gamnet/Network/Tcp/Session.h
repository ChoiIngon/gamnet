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
	std::shared_ptr<Packet> recv_packet;

	Session();
	virtual ~Session();

	virtual void OnAccept() = 0;
	virtual void OnClose(int reason) = 0;

	void Send(const std::shared_ptr<Packet>& packet);
	void Send(const char* buf, int len);
	void Send(uint32_t msg_id, const Json::Value& json);
};
}}}
#endif /* NETWORK_SESSION_H_ */
