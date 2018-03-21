#ifndef GAMNET_NETWORK_TCP_SESSION_H_
#define GAMNET_NETWORK_TCP_SESSION_H_

#include "../../Library/Json/json.h"
#include "../Session.h"
#include "Packet.h"

namespace Gamnet { namespace Network { namespace Tcp {

class Session : public Network::Session
{
public :
	struct Init
	{
		template <class T>
		T* operator() (T* session)
		{
			Network::Session::Init init;
			init(session);
			session->msg_seq = 0;
			return session;
		};
	};
	
	std::atomic<uint32_t> msg_seq;

	Session();
	virtual ~Session();
};
}}}
#endif /* NETWORK_SESSION_H_ */
