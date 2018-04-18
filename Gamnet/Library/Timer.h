#ifndef __GAMNET_LIB_TIMER_H_
#define __GAMNET_LIB_TIMER_H_

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>
#include "Singleton.h"

namespace Gamnet
{
/*!
 * \brief event timer
 
		timer will call registered callback function if time expired
		
		Sample code :<br>
  	<pre>
		Gamnet::Timer timer;
		timer.SetTimer(500, [&timer](){
			// Do Something
			timer.Resume();
		});
  	</pre>
 */
class Timer
{
	struct TimerEntry
	{
		virtual ~TimerEntry()
		{
		}
		virtual void OnExpire() = 0;
	};

	template<class FUNCTOR>
	struct TimerEntryT : public TimerEntry
	{
	    FUNCTOR functor_;
	    TimerEntryT(FUNCTOR functor) : functor_(functor) {}
	    virtual ~TimerEntryT() {}
	    virtual void OnExpire() { functor_();}
	};

	std::shared_ptr<TimerEntry> entry_;
	std::mutex lock_;
	long interval_;
	bool auto_reset_;
	boost::asio::deadline_timer deadline_timer_;

	void OnExpire(const boost::system::error_code& ec)
	{
		if (0 != ec)
		{
			return;
		}

		std::shared_ptr<TimerEntry> entry = entry_;
		{
			std::lock_guard<std::mutex> lo(lock_);
			if (nullptr == entry)
			{
				return;
			}
		}
		entry->OnExpire();
		
		if(true == auto_reset_)
		{
			Resume();
		}
	}
public :
	Timer() :
		entry_(nullptr), interval_(0), auto_reset_(false), deadline_timer_(Singleton<boost::asio::io_service>::GetInstance())
	{
	}

	Timer(boost::asio::io_service& ioService) :
		entry_(nullptr), interval_(0), auto_reset_(false), deadline_timer_(ioService)
	{	
	}

	~Timer()
	{
		deadline_timer_.expires_at(boost::posix_time::pos_infin);
		entry_ = nullptr;
	}
	/*!
		\param interval ms(1/1000 sec)
		\param functor call back funcion
	*/
	template <class FUNCTOR>
	Timer(long interval, FUNCTOR functor) :
		entry_(nullptr), interval_(interval), deadline_timer_(Singleton<boost::asio::io_service>::GetInstance())
	{
		SetTimer(interval, functor);
	}

	/*!
		\param interval ms(1/1000 sec)
		\param functor call back funcion
	*/
	template <class FUNCTOR>
	bool SetTimer(int interval, FUNCTOR functor)
	{
		interval_ = interval;
		{
			std::lock_guard<std::mutex> lo(lock_);
			entry_ = std::shared_ptr<TimerEntryT<FUNCTOR>>(new TimerEntryT<FUNCTOR>(functor));
		}
		deadline_timer_.expires_from_now(boost::posix_time::milliseconds(interval_));
		deadline_timer_.async_wait(boost::bind(&Timer::OnExpire, this, boost::asio::placeholders::error));

		return true;
	}

    /*!
     * \brief timer event will expire just onetime. if reset timer event call 'Resume'
     */
	bool Resume()
	{
		{
			std::lock_guard<std::mutex> lo(lock_);
			if(nullptr == entry_)
			{
				return false;
			}
		}
		deadline_timer_.expires_at(deadline_timer_.expires_at() + boost::posix_time::milliseconds(interval_));
		deadline_timer_.async_wait(boost::bind(&Timer::OnExpire, this, boost::asio::placeholders::error));
		return true;
	}

	void AutoReset(bool flag)
	{
		auto_reset_ = flag;
	}

	void Cancel()
	{
		deadline_timer_.cancel();
		std::lock_guard<std::mutex> lo(lock_);
		entry_ = nullptr;
	}

	template<class TIMEUNIT_T = std::chrono::milliseconds>
	static uint64_t Now()
	{
		auto now = std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<TIMEUNIT_T>(now.time_since_epoch()).count();
	}
};

class ElapseTimer
{
	std::chrono::time_point<std::chrono::high_resolution_clock> t0_;
	bool auto_reset_;
public :
	ElapseTimer() : t0_(std::chrono::high_resolution_clock::now()), auto_reset_(false)
	{
	}

	void AutoReset(bool flag)
	{
		auto_reset_ = flag;
	}

	void Reset()
	{
		t0_ = std::chrono::high_resolution_clock::now();
	}

	template<class TIMEUNIT_T = std::chrono::milliseconds>
	uint64_t Count() 
	{
		auto t1 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> fsec = t1 - t0_;
		TIMEUNIT_T diff = std::chrono::duration_cast<TIMEUNIT_T>(fsec);
		if(true == auto_reset_)
		{
			t0_ = t1;
		}
		return diff.count();
	}
};

}

#endif /* TIMER_H_ */
