#ifndef GAMNET_NETWORK_ROUTER_SESSION_H
#define GAMNET_NETWORK_ROUTER_SESSION_H

#include "MsgRouter.h"
#include "../Handler.h"
#include "../Tcp/Session.h"
#include "../Tcp/Connector.h"
#include "../Tcp/AsyncResponse.h"
#include "../../Library/Time/Time.h"
#include "../../Library/Exception.h"
#include <future>

namespace Gamnet { namespace Network { namespace Router {
						
class SyncSession;
class AsyncSession;
class Session;

class Session : public Network::Tcp::Session 
{
public :
	enum class TYPE
	{
		INVALID,
		MASTER,
		RECV
	};

	static constexpr int ASYNC_POOL_SIZE = 16;
	Session();
	virtual ~Session();
public :
	Address							router_address;
	boost::asio::ip::tcp::endpoint	remote_endpoint;
	TYPE							type;

public :		
	virtual bool Init() override;
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	virtual void Close(int reason) override;

	virtual void OnConnect();

	std::shared_ptr<Tcp::Packet> SyncSend(const std::shared_ptr<Tcp::Packet>& packet, int timeout = 5);
	using Network::Session::AsyncSend;
	
	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet) override;
	void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, const std::shared_ptr<Tcp::IAsyncResponse>& response);
private :
	Pool<SyncSession, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> sync_pool;
	std::atomic<uint64_t> async_pool_index;
	std::array<std::shared_ptr<AsyncSession>, ASYNC_POOL_SIZE> async_pool;
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

class AsyncSession : public Network::Session
{
public :
	AsyncSession();
	bool Connect(const boost::asio::ip::tcp::endpoint& endpoint);
	virtual bool Init() override;

	using Network::Session::AsyncSend;
	void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet);
	void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, const std::shared_ptr<Tcp::IAsyncResponse>& response);

	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;

	virtual void OnCreate() override {}
	virtual void OnAccept() override {}
	virtual void OnClose(int reason) override {};
	virtual void OnDestroy() override {};

	virtual void Close(int reason) override;
private :
	Tcp::Connector connector;
	void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
	
	Tcp::Packet::Protocol protocol;
	std::map<uint32_t, std::shared_ptr<Tcp::IAsyncResponse>> async_responses;
};

}}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
