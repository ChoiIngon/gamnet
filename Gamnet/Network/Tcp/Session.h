#ifndef GAMNET_NETWORK_TCP_SESSION_H_
#define GAMNET_NETWORK_TCP_SESSION_H_

#include "../../Library/Time/Timer.h"
#include "../Session.h"
#include "Packet.h"

namespace Gamnet { namespace Network { namespace Tcp {
class Session : public Network::Session
{
public:
	enum {
		RELIABLE_PACKET_QUEUE_SIZE = 100
	};
	enum class State
	{
		Invalid,
		AfterCreate,
		AfterAccept
	};
public :
	Session();
	virtual ~Session();

	bool	 handover_safe;
	uint32_t recv_seq;
	uint32_t send_seq;

	time_t	 last_recv_time;

	std::shared_ptr<Packet>								recv_packet;
	std::deque<std::shared_ptr<Packet>>					send_packets;

	std::string	session_token;
	State		session_state;

	virtual bool Init() override;
	virtual void Clear() override;
	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;
	virtual void Close(int reason) override;

	virtual void AsyncSend(const std::shared_ptr<Packet>& packet);

	void SetExpire(int timeout);
	static std::string GenerateSessionToken(uint32_t session_key);
	void OnIdleTimeout();
private :

	Time::Timer expire_timer;
};

}}}
#endif /* NETWORK_SESSION_H_ */
