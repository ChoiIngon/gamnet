#include "Timer.h"
#include "../Pool.h"
#include "../Singleton.h"
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Gamnet { namespace Time {

struct InitFunctor
{
    template<class T>
    T* operator() (T* timer)
    {
        return timer;
    }
};

struct ReleaseFunctor
{
    template<class T>
    T* operator() (T* timer)
    {
        if (nullptr != timer)
        {
            timer->Cancel();
        }
        return timer;
    }
};

static std::atomic<uint32_t> TIMER_SEQ = 1;
static Pool<Timer, std::mutex, InitFunctor, ReleaseFunctor> timer_pool(std::numeric_limits<uint32_t>::max());
static Pool<RepeatTimer, std::mutex, InitFunctor, ReleaseFunctor> repeattimer_pool(std::numeric_limits<uint32_t>::max());

std::shared_ptr<Timer> Timer::Create()
{
    auto timer = timer_pool.Create();
#ifdef _DEBUG
    timer->timer_seq = TIMER_SEQ++;
#endif
    return timer;
}

Timer::Timer()
	: entry(nullptr), deadline_timer(Singleton<boost::asio::io_context>::GetInstance())
{
}

Timer::~Timer()
{
    deadline_timer.cancel();
	deadline_timer.expires_at(boost::posix_time::pos_infin);
}

void Timer::OnExpire(const boost::system::error_code& ec)
{
	if (0 != ec.value())
	{
		return;
	}

	std::lock_guard<std::recursive_mutex> lo(lock);
	if (nullptr == entry)
	{
		return;
	}

	entry->OnExpire();
	entry = nullptr;
}

void Timer::Cancel()
{
    std::lock_guard<std::recursive_mutex> lo(lock);
    deadline_timer.cancel();
    entry = nullptr;
}

std::shared_ptr<RepeatTimer> RepeatTimer::Create()
{
    auto timer = repeattimer_pool.Create();
#ifdef _DEBUG
    timer->timer_seq = TIMER_SEQ++;
#endif
    return timer;
}

RepeatTimer::RepeatTimer()
    : interval(0), Timer()
{
}

RepeatTimer::~RepeatTimer()
{
}

void RepeatTimer::OnExpire(const boost::system::error_code& ec)
{
    if (0 != ec.value())
    {
        return;
    }

    std::lock_guard<std::recursive_mutex> lo(lock);
    if (nullptr == entry)
    {
        return;
    }

    entry->OnExpire();

    deadline_timer.expires_at(deadline_timer.expires_at() + boost::posix_time::milliseconds(interval));
    deadline_timer.async_wait(boost::bind(&RepeatTimer::OnExpire, this, boost::asio::placeholders::error));
}

ElapseTimeCounter::ElapseTimeCounter()
	: t0_(std::chrono::high_resolution_clock::now())
{
}

void ElapseTimeCounter::Reset()
{
	t0_ = std::chrono::high_resolution_clock::now();
}

}}