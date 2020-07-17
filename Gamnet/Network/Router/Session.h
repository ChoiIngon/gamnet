#ifndef GAMNET_NETWORK_ROUTER_SESSION_H
#define GAMNET_NETWORK_ROUTER_SESSION_H

#include "MsgRouter.h"
#include "../Tcp/Session.h"
#include "../Tcp/Connector.h"
#include "../../Library/Time/Time.h"
#include <future>

namespace Gamnet { namespace Network { namespace Router {

class SyncSession;
class AsyncSession;

class Session : public Network::Tcp::Session 
{
public :
	enum TYPE
	{
		INVALID,
		MASTER,
		SEND,
		RECV
	};

	struct Timeout
	{
		uint32_t timeout_seq;
		int expire_time;
		std::function<void(const std::shared_ptr<Tcp::Packet>&)> on_receive;
		std::function<void()> on_timeout;
	};

	Session();
	virtual ~Session();
public :
	Address							router_address;
	boost::asio::ip::tcp::endpoint	remote_endpoint;
	TYPE							type;
	std::atomic_uint32_t			timeout_seq;

public :		
	virtual bool Init() override;
	virtual void OnCreate() override;
	virtual void OnAccept() override;
	virtual void OnConnect();
	virtual void OnClose(int reason) override;
	virtual void OnDestroy() override;
	virtual void Close(int reason) override;
	virtual const std::shared_ptr<Timeout> FindTimeout(uint32_t seq) { return nullptr; }
	
	using Network::Session::AsyncSend;
	
	std::shared_ptr<Tcp::Packet> SyncSend(const std::shared_ptr<Tcp::Packet>& packet, int timeout = 5);
	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet) override;
			void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, std::function<void(const std::shared_ptr<Tcp::Packet>&)> onReceive, std::function<void()> onTimeout, int timeout);

	//const std::shared_ptr<ResponseTimeout> FindResponseTimeout(uint32_t msgSEQ);
private :
	Pool<SyncSession, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> syncsession_pool;
	Pool<AsyncSession, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> asyncsession_pool;
};

class SyncSession : public Network::Session
{
public:
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

private:
	Time::Timer expire_timer;
	Tcp::Connector connector;
	void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
};

class AsyncSession : public Session
{
public :
	struct Factory
	{
		AsyncSession* operator()();
	};

public :
	AsyncSession();
	bool Connect(const boost::asio::ip::tcp::endpoint& endpoint);
	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet) override;
			void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, std::function<void(const std::shared_ptr<Tcp::Packet>&)>& onReceive, std::function<void()>& onTimeout, int timeout);
	virtual const std::shared_ptr<Timeout> FindTimeout(uint32_t seq) override;
private :
	
	Tcp::Connector connector;
	void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
	void SetTimeout(const std::shared_ptr<Timeout>& timeout);
	void OnTimeout();

	Time::Timer expire_timer;
	
	Pool<Timeout> timeout_pool;
	std::map<uint32_t, std::shared_ptr<Timeout>> timeouts;
};

class LocalSession : public Session
{
public :
	virtual void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet) override;
};

}}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
