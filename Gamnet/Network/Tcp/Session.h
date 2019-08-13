#ifndef GAMNET_NETWORK_TCP_SESSION_H_
#define GAMNET_NETWORK_TCP_SESSION_H_

#include "../../Library/Json/json.h"
#include "../Session.h"
#include "Packet.h"

namespace Gamnet { namespace Network { namespace Tcp {

class Session : public Network::Session
{
public:
	enum {
		RELIABLE_PACKET_QUEUE_SIZE = 50
	};
public :
	Session();
	Session(boost::asio::io_service& ioService);
	virtual ~Session();

	virtual bool Init() override;
	virtual void Clear() override;
	bool AsyncSend(const std::shared_ptr<Packet>& packet);
	bool handover_safe;
	uint32_t recv_seq;
	uint32_t send_seq;
	std::deque<std::shared_ptr<Packet>>	send_packets;
};
}}}
#endif /* NETWORK_SESSION_H_ */
