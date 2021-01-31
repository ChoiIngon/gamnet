#include "AsyncResponse.h"
#include "Session.h"
#include "../../Library/Singleton.h"

namespace Gamnet { namespace Network { namespace Router {

std::atomic_uint32_t response_seq;

IAsyncResponse::IAsyncResponse()
	: seq(0)
	, handler(nullptr)
	, timer(Singleton<boost::asio::io_context>::GetInstance())
	, timeout(0)
	, on_exception(nullptr)
{
}

IAsyncResponse::~IAsyncResponse()
{
}

void IAsyncResponse::Init()
{
	seq = ++response_seq;
}

void IAsyncResponse::Clear()
{
	seq = 0;
	handler = nullptr;
	timeout = 0;
	on_exception = nullptr;
	timer.cancel();
}

void IAsyncResponse::OnException(const Gamnet::Exception& e)
{
	if (nullptr != on_exception)
	{
		on_exception(handler, e);
	}
}

void IAsyncResponse::StartTimer(std::function<void()> expire)
{
	std::shared_ptr<Session> session = this->session.lock();
	assert(nullptr != session);

	timer.expires_from_now(boost::posix_time::milliseconds(timeout));
	timer.async_wait(session->Bind([this, expire](const boost::system::error_code& ec) {
		if (0 != ec.value())
		{
			return;
		}
		OnException(GAMNET_EXCEPTION(ErrorCode::ResponseTimeoutError));
		expire();
	}));
}

void IAsyncResponse::StopTimer()
{
	timer.cancel();
	on_exception = nullptr;
}

}}}