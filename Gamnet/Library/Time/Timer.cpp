#include "Timer.h"
#include "../Pool.h"
#include "../Singleton.h"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Gamnet { namespace Time {

	struct InitFunctor
	{
		Timer* operator() (Timer* timer)
		{
			return timer;
		}
	};

	struct ReleaseFunctor
	{
		Timer* operator() (Timer* timer)
		{
			if (nullptr != timer)
			{
				timer->Cancel();
			}
			return timer;
		}
	};
	static Pool<Timer, std::mutex, InitFunctor, ReleaseFunctor> pool(std::numeric_limits<uint32_t>::max());
	std::shared_ptr<Timer> Timer::Create()
	{
		return pool.Create();
	}

	static std::atomic<uint32_t> TIMER_SEQ;
Timer::Timer()
	: interval(0), auto_reset(false), entry(nullptr), deadline_timer(Singleton<boost::asio::io_context>::GetInstance())
#ifdef _DEBUG
	, timer_seq(++TIMER_SEQ)
#endif
{
}

Timer::~Timer()
{
	deadline_timer.expires_at(boost::posix_time::pos_infin);
}

void Timer::OnExpire(const boost::system::error_code& ec)
{
	if (0 != ec.value())
	{
		return;
	}

	std::shared_ptr<TimerEntry> entry = nullptr;
	{
		std::lock_guard<std::mutex> lo(lock);
		entry = this->entry;
		if (nullptr == entry)
		{
			return;
		}
	}
	entry->OnExpire();

	if (true == auto_reset)
	{
		Resume();
	}
}

bool Timer::Resume()
{
	std::lock_guard<std::mutex> lo(lock);
	if (0 == interval)
	{
		return false;
	}

	if (nullptr == entry)
	{
		return false;
	}

	deadline_timer.expires_at(deadline_timer.expires_at() + boost::posix_time::milliseconds(interval));
	deadline_timer.async_wait(boost::bind(&Timer::OnExpire, this, boost::asio::placeholders::error));
	return true;
}

void Timer::AutoReset(bool flag)
{
	auto_reset = flag;
}

void Timer::Cancel()
{
	deadline_timer.cancel();
	std::lock_guard<std::mutex> lo(lock);
	entry = nullptr;
}

ElapseTimer::ElapseTimer()
	: t0_(std::chrono::high_resolution_clock::now()), auto_reset_(false)
{
}

void ElapseTimer::AutoReset(bool flag)
{
	auto_reset_ = flag;
}

void ElapseTimer::Reset()
{
	t0_ = std::chrono::high_resolution_clock::now();
}

}}