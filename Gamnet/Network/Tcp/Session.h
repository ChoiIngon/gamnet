#ifndef GAMNET_NETWORK_TCP_SESSION_H_
#define GAMNET_NETWORK_TCP_SESSION_H_

#include "../../Library/Json/json.h"
#include "../HandlerContainer.h"
#include "../Session.h"
#include "Packet.h"
namespace Gamnet { namespace Network { namespace Tcp {
class Session : public Network::Session
{
public:
	enum {
		RELIABLE_PACKET_QUEUE_SIZE = 100
	};
public :
	Session();
	virtual ~Session();

	bool handover_safe;
	uint32_t recv_seq;
	uint32_t send_seq;

	std::shared_ptr<Packet> recv_packet;
	std::deque<std::shared_ptr<Packet>>	send_packets;

	std::string											session_token;
	HandlerContainer									handler_container;
	virtual bool Init() override;
	virtual void Clear() override;
	void AsyncSend(const std::shared_ptr<Packet> packet);
	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;
	static std::string GenerateSessionToken(uint32_t session_key);
};

}}}
#endif /* NETWORK_SESSION_H_ */
