#ifndef GAMNET_NETWORK_ROUTER_SESSION_H
#define GAMNET_NETWORK_ROUTER_SESSION_H

#include "MsgRouter.h"
#include "../Tcp/Session.h"
#include "../Tcp/Connector.h"
#include "../Handler.h"
#include "../../Library/Time/Time.h"
#include "../../Library/Exception.h"
#include <future>

namespace Gamnet { namespace Network { namespace Router {
						
class SyncSession;
class AsyncSession;
class Session;

struct IResponseHandler
{
	IResponseHandler();
	virtual void OnReceive(const std::shared_ptr<Tcp::Packet>&) = 0;

	uint32_t msg_seq;
	int expire_time;
	std::function<void(const Exception&)> on_exception;
};

template <class MsgType>
struct ResponseHandler : IResponseHandler
{
	//typedef void(IHandler::* FunctionType)(const MsgType&);

	struct Init
	{
		ResponseHandler* operator() (ResponseHandler* resHandler)
		{
			return resHandler;
		}
	};

	struct Release
	{
		ResponseHandler* operator() (ResponseHandler* resHandler) {
			resHandler->msg_seq = 0;
			resHandler->expire_time = 0;
			resHandler->on_receive = nullptr;
			resHandler->on_exception = nullptr;
			return resHandler;
		};
	};

	template <class FunctionType>
	ResponseHandler(FunctionType onReceive)
		: on_receive(onReceive)
	{
	}
	virtual void OnReceive(const std::shared_ptr<Tcp::Packet>& packet)
	{
		MsgType msg;
		if (false == Gamnet::Network::Tcp::Packet::Load(msg, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		on_receive(msg);
	}

	std::function<void(const MsgType&)> on_receive;
};

class Session : public Network::Tcp::Session 
{
public :
	enum class TYPE
	{
		INVALID,
		MASTER,
		RECV
	};

	enum CONST_NUMBER {
		ASYNC_POOL_SIZE = 16
	};
	Session();
	virtual ~Session();
public :
	Address							router_address;
	boost::asio::ip::tcp::endpoint	remote_endpoint;
	TYPE							type;
	std::atomic_uint32_t			msg_seq;

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
	
	void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, std::shared_ptr<IResponseHandler>& responseHandler, std::function<void(const Exception&)>& onException, int seconds);
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

class AsyncSession : public Network::Tcp::Session
{
public :
	AsyncSession();
	bool Connect(const boost::asio::ip::tcp::endpoint& endpoint);
	virtual bool Init() override;

	using Network::Session::AsyncSend;
	void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet);
	void AsyncSend(const std::shared_ptr<Tcp::Packet>& packet, std::shared_ptr<IResponseHandler>& responseHandler, std::function<void(const Exception&)>& onException, int seconds);

	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;

	virtual void OnCreate() override {}
	virtual void OnAccept() override {}
	virtual void OnClose(int reason) override {};
	virtual void OnDestroy() override {};

	virtual void Close(int reason) override;
private :

	const std::shared_ptr<IResponseHandler> FindResponseHandler(uint32_t seq);
	
	Tcp::Connector connector;
	void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
	void SetTimeout(const std::shared_ptr<IResponseHandler>& timeout);
	void OnTimeout();

	Time::Timer expire_timer;
	
	//Pool<ResponseHandler, Policy::nolock, ResponseHandler::Init, ResponseHandler::Release> response_handler_pool;
	Tcp::Packet::Protocol protocol;
	std::map<uint32_t, std::shared_ptr<IResponseHandler>> response_handlers;
};

}}} /* namespace Gamnet */

#endif /* SERVERSESSION_H_ */
