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
	virtual bool Init() override;

	virtual void OnCreate() override {}
	virtual void OnAccept()	override {}
	virtual void OnClose(int reason) override {}
	virtual void OnDestroy() override {}

	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;

	virtual void Close(int reason) override;
};

}}}
#endif /* SESSION_H_ */
