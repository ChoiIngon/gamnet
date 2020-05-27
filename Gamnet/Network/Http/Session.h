#ifndef GAMNET_NETWORK_HTTP_SESSION_H_
#define GAMNET_NETWORK_HTTP_SESSION_H_

#include "../Tcp/Session.h"
#include "Response.h"

namespace Gamnet { namespace Network { namespace Http {

class Session : public Network::Session 
{
	std::shared_ptr<Buffer> recv_buffer;
public:
	Session();
	virtual ~Session();
	
	void Send(const Response& res);

	virtual void OnCreate() {}
	virtual void OnAccept()	{}
	virtual void OnClose(int reason) {}
	virtual void OnDestroy() {}

	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;
};

}}}
#endif /* SESSION_H_ */
