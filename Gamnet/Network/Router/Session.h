#ifndef GAMNET_NETWORK_ROUTER_SESSION_H
#define GAMNET_NETWORK_ROUTER_SESSION_H

#include "MsgRouter.h"
#include "AsyncResponse.h"
#include "../Handler.h"
#include "../Tcp/Session.h"
#include "../Tcp/Connector.h"
#include "../../Library/Time/Time.h"
#include "../../Library/Exception.h"
#include <future>

namespace Gamnet { namespace Network { namespace Router {

class SyncSession;

class Session : public Network::Tcp::Session 
{
public :
	Session();
	virtual ~Session();
public :
	Address							router_address;
	boost::asio::ip::tcp::endpoint	remote_endpoint;

public :		
	virtual bool Init() override;
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	virtual void Close(int reason) override;

	virtual void OnConnect();

	std::shared_ptr<Tcp::Packet> SyncSend(const std::shared_ptr<Tcp::Packet>& packet, int timeout = 5);

	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet) override;
	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, const std::shared_ptr<IAsyncResponse>& response);

	std::map<uint32_t, std::shared_ptr<IAsyncResponse>> async_responses;
private :
	Pool<SyncSession, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> sync_pool;
};

class LocalSession : public Session
{
public:
	LocalSession();
	virtual ~LocalSession();

	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet) override;
};

class SyncSession : public Network::Session
{
public:
	SyncSession();
	~SyncSession();

	virtual bool Init() override;

	bool Connect(const boost::asio::ip::tcp::endpoint& endpoint);

	std::shared_ptr<Tcp::Packet> SyncRead(int timeout);

	virtual void OnCreate() override {}
	virtual void OnAccept() override {}
	virtual void OnClose(int reason) override {};
	virtual void OnDestroy() override {};
	virtual void Close(int reason);
protected:
	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override {}

private:
	Time::Timer expire_timer;
	Tcp::Connector connector;
	void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
};

}}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
