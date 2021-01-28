#ifndef GAMNET_NETWORK_TCP_ASYNC_RESPONSE_H_
#define GAMNET_NETWORK_TCP_ASYNC_RESPONSE_H_

#include <memory>
#include "Packet.h"
#include "../Handler.h"
#include "../../Library/Pool.h"
#include "../../Library/Time/Timer.h"

namespace Gamnet { namespace Network { namespace Tcp {

struct IAsyncResponse
{
	IAsyncResponse();
	virtual ~IAsyncResponse();

	virtual void Init();
	virtual void Clear();
	virtual void OnReceive(const std::shared_ptr<Packet>&) = 0;
	virtual void OnException(const Gamnet::Exception&) = 0;
	virtual void OnExpire(std::function<void()> expire) = 0;
	
	uint32_t seq;
	int timeout;
	std::shared_ptr<IHandler> handler;
};

template <class MsgType>
struct AsyncResponse : IAsyncResponse
{
	typedef void(IHandler::*OnReceiveFuncType)(const MsgType&);
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
		, on_exception(nullptr)
	{
	}

	virtual void Clear() override
	{
		IAsyncResponse::Clear();
		on_receive = nullptr;
		on_exception = nullptr;
		timer.Cancel();
	}

	virtual void OnReceive(const std::shared_ptr<Packet>& packet) override
	{
		timer.Cancel();
		MsgType msg;
		
		if (false == Packet::Load(msg, packet))
		{
			assert(nullptr != on_receive);
			if (nullptr != on_exception)
			{
				on_exception(handler, GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail"));
				return;
			}
		}
		on_receive(handler, msg);
	}

	virtual void OnException(const Gamnet::Exception& e) override
	{
		if (nullptr != on_exception)
		{
			on_exception(handler, e);
		}
	}

	virtual void OnExpire(std::function<void()> expire) override
	{
		timer.AutoReset(false);
		timer.SetTimer(timeout, [this, expire]() {
			if (nullptr != on_exception)
			{
				on_exception(handler, GAMNET_EXCEPTION(ErrorCode::ResponseTimeoutError));
			}
			expire();
		});
	}

	Time::Timer timer;
	
	std::function<void(const std::shared_ptr<IHandler>&, const MsgType&)> on_receive;
	std::function<void(const std::shared_ptr<IHandler>&, const Gamnet::Exception& e)> on_exception;

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
