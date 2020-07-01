#ifndef GAMNET_NETWORK_ROUTER_SESSION_H
#define GAMNET_NETWORK_ROUTER_SESSION_H

#include "MsgRouter.h"
#include "../Tcp/Session.h"
#include "../../Library/Time/Time.h"

namespace Gamnet { namespace Network { namespace Router {

class Session : public Network::Tcp::Session 
{
public:
	struct ResponseTimeout
	{
		time_t expire_time;
		std::function<void()> on_timeout;
	};

public :
	Session();
	virtual ~Session();

public :
	Address							router_address;
private :
	Time::Timer expire_timer;
	std::map<uint64_t, std::shared_ptr<ResponseTimeout>> response_timeouts;

public :		
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnConnect();
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	virtual void Close(int reason) override;

	using Network::Session::AsyncSend;
	void AsyncSend(const std::shared_ptr<Tcp::Packet> packet, uint32_t responseMsgID, std::shared_ptr<IHandler> handler, int seconds, std::function<void()> onTimeout);

	const std::shared_ptr<ResponseTimeout> FindResponseTimeout(uint32_t msgSEQ);
private :
	void OnResponseTimeout();
};

class LocalSession : public Session
{
public :
	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet> packet) override;
};
}}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
