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

static std::atomic<uint32_t> TIMER_SEQ;

TimerBase::TimerBase()
    : entry(nullptr)
    , deadline_timer(Singleton<boost::asio::io_context>::GetInstance())
#ifdef _DEBUG
    , timer_seq(++TIMER_SEQ)
#endif
{
}

TimerBase::~TimerBase()
{
    Cancel();
}

void TimerBase::Cancel()
{
    std::lock_guard<std::recursive_mutex> lo(lock);
    if(nullptr == entry)
    {
        return;
    }

    deadline_timer.cancel();
    deadline_timer.expires_at(boost::posix_time::pos_infin);
    entry = nullptr;
}

static Pool<Timer, std::mutex, InitFunctor, ReleaseFunctor> timer_pool(std::numeric_limits<uint32_t>::max());

std::shared_ptr<Timer> Timer::Create()
{
    return timer_pool.Create();
}

Timer::Timer()
    : TimerBase()
{
}

Timer::~Timer()
{
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

static Pool<RepeatTimer, std::mutex, InitFunctor, ReleaseFunctor> repeattimer_pool(std::numeric_limits<uint32_t>::max());

std::shared_ptr<RepeatTimer> RepeatTimer::Create()
{
    return repeattimer_pool.Create();
}

RepeatTimer::RepeatTimer()
    : TimerBase(), interval(0)
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
