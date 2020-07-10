#ifndef GAMNET_NETWORK_ROUTER_SESSION_H
#define GAMNET_NETWORK_ROUTER_SESSION_H

#include "MsgRouter.h"
#include "../Tcp/Session.h"
#include "../Tcp/Connector.h"
#include "../../Library/Time/Time.h"
#include <future>

namespace Gamnet { namespace Network { namespace Router {

class SyncSession : public Network::Session
{
public :
	SyncSession();
	~SyncSession();

	bool Connect(const boost::asio::ip::tcp::endpoint& endpoint);
		
	std::shared_ptr<Tcp::Packet> SyncRead(int timeout);

	virtual void OnCreate() override {}
	virtual void OnAccept() override {}
	virtual void OnClose(int reason) override {};
	virtual void OnDestroy() override {};
	virtual void Close(int reason);
protected:
	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override {}

private :
	Time::Timer expire_timer;
	Tcp::Connector connector;
	boost::asio::ip::tcp::endpoint remote_endpoint;
	void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
	void OnSyncRead(std::promise<std::shared_ptr<Tcp::Packet>>* promise, int timeout);
};

class Session : public Network::Tcp::Session 
{
public :
	Session();
	virtual ~Session();

public :
	Address							router_address;
private :
	Time::Timer expire_timer;
public :		
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnConnect();
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	virtual void Close(int reason) override;

	using Network::Session::AsyncSend;
	
	std::shared_ptr<Tcp::Packet> SyncSend(const std::shared_ptr<Tcp::Packet>& packet, int timeout = 5);
	//const std::shared_ptr<ResponseTimeout> FindResponseTimeout(uint32_t msgSEQ);
private :
	//void OnResponseTimeout();
	//void OnConnectHandler(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
	Pool<SyncSession, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> pool;
};

class LocalSession : public Session
{
public :
	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet> packet) override;
};

}}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
