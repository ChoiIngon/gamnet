#ifndef GAMNET_NETWORK_TCP_SESSION_H_
#define GAMNET_NETWORK_TCP_SESSION_H_

#include "../../Library/Json/json.h"
#include "../Session.h"
#include "Packet.h"

namespace Gamnet { namespace Network { namespace Tcp {

class Session : public Network::Session
{
public :
	Session();
	Session(boost::asio::io_service& ioService);
	virtual ~Session();

	virtual bool Init() override;
};
}}}
#endif /* NETWORK_SESSION_H_ */
