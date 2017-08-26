#ifndef GAMNET_NETWORK_HTTP_SESSION_H_
#define GAMNET_NETWORK_HTTP_SESSION_H_

#include "../Session.h"
#include "Response.h"

namespace Gamnet {
namespace Network {
namespace Http {

class Session : public Network::Session {
public:
	Session();
	virtual ~Session();

	int Send(const Response& res);

	virtual void OnCreate() {}
	virtual void OnAccept()	{}
	virtual void OnClose(int reason) {}
};

} /* namespace Http */
} /* namespace Gamnet */
}
#endif /* SESSION_H_ */
