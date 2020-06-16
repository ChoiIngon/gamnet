#ifndef __GAMNET_LIB_TIMER_H_
#define __GAMNET_LIB_TIMER_H_

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>
#include <mutex>
#include "Singleton.h"

namespace Gamnet { namespace Time {

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
public :
	static std::shared_ptr<Timer> Create();
private :
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
	    FUNCTOR functor;
	    TimerEntryT(FUNCTOR functor) : functor(functor) {}
	    virtual ~TimerEntryT() {}
	    virtual void OnExpire() { functor(); }
	};

	std::mutex lock;
	long interval;
	bool auto_reset;
	std::shared_ptr<TimerEntry> entry;
	boost::asio::deadline_timer deadline_timer;

	void OnExpire(const boost::system::error_code& ec);
public :
	Timer();
	~Timer();

	/*!
		\param interval ms(1/1000 sec)
		\param functor call back funcion
	*/
	template <class FUNCTOR>
	void SetTimer(int interval, FUNCTOR functor)
	{
		this->interval = interval;
		{
			std::lock_guard<std::mutex> lo(lock);
			entry = std::make_shared<TimerEntryT<FUNCTOR>>(functor);
		}
		deadline_timer.expires_from_now(boost::posix_time::milliseconds(this->interval));
		deadline_timer.async_wait(boost::bind(&Timer::OnExpire, this, boost::asio::placeholders::error));
	}

    /*!
     * \brief timer event will expire just onetime. if reset timer event call 'Resume'
     */
	bool Resume();
	void AutoReset(bool flag);
	void Cancel();
};

class ElapseTimer
{
	std::chrono::time_point<std::chrono::high_resolution_clock> t0_;
	bool auto_reset_;
public :
	ElapseTimer();
	void AutoReset(bool flag);
	void Reset();

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

enum WeekDay
{
	Sunday,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday
};

class DateTime
{
private :
	WeekDay weekday;
	int		year;
	int		month;
	int		day;
	int		hour;
	int		minute;
	int		second;
public :
	static const DateTime MaxValue;
	static const DateTime MinValue;

	DateTime();
	DateTime(time_t unixTimeStamp);
	DateTime(int year, int month, int day, int hour, int minute, int second);
	DateTime(const std::string& date);

	bool operator < (const DateTime& rhs) const;
	bool operator == (const DateTime& rhs) const;
};

DateTime operator + (const DateTime& lhs, const DateTime& rhs);
DateTime operator + (const DateTime& lhs, int seconds);
DateTime operator - (const DateTime& lhs, const DateTime& rhs);
DateTime operator - (const DateTime& lhs, int seconds);

std::string FromUnixtime(time_t now);

template<class TIMEUNIT_T = std::chrono::seconds>
static uint64_t Now()
{
	auto now = std::chrono::system_clock::now();
	return std::chrono::duration_cast<TIMEUNIT_T>(now.time_since_epoch()).count();
}

}}

#endif /* TIMER_H_ */
