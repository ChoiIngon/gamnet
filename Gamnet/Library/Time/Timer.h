#ifndef __GAMNET_LIB_TIMER_H_
#define __GAMNET_LIB_TIMER_H_

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>
#include <mutex>
#include "DateTime.h"

namespace Gamnet { namespace Time {

class Timer
{
protected :
    struct TimerEntry
    {
        virtual ~TimerEntry() {}
        virtual void OnExpire() = 0;
    };

    template<class EXPIRE_HANDLER>
    struct TimerEntryT : public TimerEntry
    {
        EXPIRE_HANDLER expire_handler;
        TimerEntryT(EXPIRE_HANDLER handler) : expire_handler(handler) {}
        virtual ~TimerEntryT() {}
        virtual void OnExpire() { expire_handler(); }
    };

public :
	static std::shared_ptr<Timer> Create();

public :
#ifdef _DEBUG
    uint32_t timer_seq;
#endif
protected :
	std::recursive_mutex lock;
	std::shared_ptr<TimerEntry> entry;
	boost::asio::deadline_timer deadline_timer;

public :
	Timer();
	virtual ~Timer();

	template <class EXPIRE_HANDLER>
	void ExpireFromNow(long interval, const EXPIRE_HANDLER& handler)
	{
		std::lock_guard<std::recursive_mutex> lo(lock);
		this->entry = std::make_shared<TimerEntryT<EXPIRE_HANDLER>>(handler);
		this->deadline_timer.expires_from_now(boost::posix_time::milliseconds(interval));
		this->deadline_timer.async_wait(boost::bind(&Timer::OnExpire, this, boost::asio::placeholders::error));
	}

	template <class EXPIRE_HANDLER>
	void ExpireAt(const DateTime& datetime, const EXPIRE_HANDLER& handler)
	{
		std::lock_guard<std::recursive_mutex> lo(lock);
		this->entry = std::make_shared<TimerEntryT<EXPIRE_HANDLER>>(handler);
        this->deadline_timer.expires_at((boost::posix_time::ptime&)datetime.UTC());
        this->deadline_timer.async_wait(boost::bind(&Timer::OnExpire, this, boost::asio::placeholders::error));
	}

	void Cancel();

protected:
    virtual void OnExpire(const boost::system::error_code& ec);
};

class RepeatTimer : public Timer
{
private :
    long interval;

public :
    static std::shared_ptr<RepeatTimer> Create();

    RepeatTimer();
    virtual ~RepeatTimer();

    template <class EXPIRE_HANDLER>
    void ExpireRepeat(long interval, const EXPIRE_HANDLER& handler)
    {
        std::lock_guard<std::recursive_mutex> lo(lock);
        this->entry = std::make_shared<TimerEntryT<EXPIRE_HANDLER>>(handler);
        this->deadline_timer.expires_from_now(boost::posix_time::milliseconds(interval));
        this->deadline_timer.async_wait(boost::bind(&RepeatTimer::OnExpire, this, boost::asio::placeholders::error));
        this->interval = interval;
    }

protected:
    virtual void OnExpire(const boost::system::error_code& ec) override;
};

class ElapseTimeCounter
{
	std::chrono::time_point<std::chrono::high_resolution_clock> t0_;
public :
    ElapseTimeCounter();
	void Reset();

	template<class TIMEUNIT_T = std::chrono::milliseconds>
	uint64_t Count()
	{
		auto t1 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> fsec = t1 - t0_;
		TIMEUNIT_T diff = std::chrono::duration_cast<TIMEUNIT_T>(fsec);
		return diff.count();
	}
};

}}

#endif /* TIMER_H_ */
