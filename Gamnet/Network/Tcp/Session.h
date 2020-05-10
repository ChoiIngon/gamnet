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
		RELIABLE_PACKET_QUEUE_SIZE = 100
	};
public :
	Session();
	virtual ~Session();

	bool handover_safe;
	uint32_t recv_seq;
	uint32_t send_seq;
	std::deque<std::shared_ptr<Packet>>	send_packets;

	virtual bool Init() override;
	virtual void Clear() override;
	void AsyncSend(const std::shared_ptr<Packet> packet);
};

class SessionManager
{
	std::mutex										_lock;
	std::map<uint32_t, std::shared_ptr<Session>>	_sessions;
public:
	SessionManager();
	~SessionManager();

	bool Init();
	bool Add(uint32_t key, const std::shared_ptr<Session>& session);
	void Remove(uint32_t key);
	std::shared_ptr<Session> Find(uint32_t key);
	size_t Size();
};

}}}
#endif /* NETWORK_SESSION_H_ */
