#ifndef GAMNET_NETWORK_TCP_ASYNC_RESPONSE_H_
#define GAMNET_NETWORK_TCP_ASYNC_RESPONSE_H_

#include <memory>
#include "../Tcp/Packet.h"
#include "../../Library/Pool.h"
#include "../../Library/Time/Timer.h"
#include "Address.h"

namespace Gamnet { namespace Network { 

struct IHandler;

namespace Router {

class Session;

struct IAsyncResponse
{
	IAsyncResponse();
	virtual ~IAsyncResponse();

	virtual void Init();
	virtual void Clear();
	virtual void OnReceive(const Address&, const std::shared_ptr<Tcp::Packet>&) = 0;
	virtual void OnException(const Gamnet::Exception& e);
	void StartTimer(std::function<void()> expire);
	void StopTimer();

	uint32_t					seq;
	std::shared_ptr<IHandler>	handler;
	std::weak_ptr<Session>		session;
	boost::asio::deadline_timer	timer;
	int							timeout;
	std::function<void(const std::shared_ptr<IHandler>&, const Gamnet::Exception& e)> on_exception;
};

template <class MsgType>
struct AsyncResponse : IAsyncResponse
{
	typedef void(IHandler::*OnReceiveFuncType)(const Address&, const MsgType&);
	typedef void(IHandler::*OnExceptionFuncType)(const Gamnet::Exception&);

	struct InitFunctor
	{
		AsyncResponse* operator() (AsyncResponse* ptr)
		{
			ptr->Init();
			return ptr;
		}
	};

	struct ReleaseFunctor
	{
		AsyncResponse* operator() (AsyncResponse* ptr)
		{
			ptr->Clear();
			return ptr;
		}
	};

	AsyncResponse()
		: on_receive(nullptr)
	{
	}

	virtual void Clear() override
	{
		IAsyncResponse::Clear();
		on_receive = nullptr;
	}

	virtual void OnReceive(const Address& address, const std::shared_ptr<Tcp::Packet>& packet) override
	{
		MsgType msg;
		
		if (false == Tcp::Packet::Load(msg, packet))
		{
			assert(nullptr != on_receive);
			if (nullptr != on_exception)
			{
				on_exception(handler, GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail"));
				return;
			}
		}
		on_receive(handler, address, msg);
	}

	std::function<void(const std::shared_ptr<IHandler>&, const Address&, const MsgType&)> on_receive;

	static std::shared_ptr<AsyncResponse<MsgType>> Create();
	static Pool<AsyncResponse<MsgType>, std::mutex, typename AsyncResponse<MsgType>::InitFunctor, typename AsyncResponse<MsgType>::ReleaseFunctor> pool;
};

template <class MsgType>
Pool<AsyncResponse<MsgType>, std::mutex, typename AsyncResponse<MsgType>::InitFunctor, typename AsyncResponse<MsgType>::ReleaseFunctor> AsyncResponse<MsgType>::pool(65535);

template <class MsgType>
std::shared_ptr<AsyncResponse<MsgType>> AsyncResponse<MsgType>::Create()
{
	return AsyncResponse<MsgType>::pool.Create();
}

}}} /* namespace Gamnet */
#endif /* HANDLER_H_ */
