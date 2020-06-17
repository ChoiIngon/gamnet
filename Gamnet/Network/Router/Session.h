#ifndef GAMNET_NETWORK_ROUTER_SESSION_H
#define GAMNET_NETWORK_ROUTER_SESSION_H

#include "MsgRouter.h"
#include "../Tcp/Session.h"
#include "../../Library/Time/Time.h"

namespace Gamnet { namespace Network { namespace Router {

class Session : public Network::Tcp::Session 
{
public:

	Session();
	virtual ~Session();

	Address							router_address;
		
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnConnect();
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	virtual void Close(int reason) override;
};

class LocalSession : public Session
{
public :
	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet> packet) override;
};
}}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
