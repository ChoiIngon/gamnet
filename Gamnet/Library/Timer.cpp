#include "Timer.h"
#include "Pool.h"

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

Timer::Timer()
	: interval(0), auto_reset(false), entry(nullptr), deadline_timer(Singleton<boost::asio::io_service>::GetInstance())
{
}

Timer::~Timer()
{
	deadline_timer.expires_at(boost::posix_time::pos_infin);
}

void Timer::OnExpire(const boost::system::error_code& ec)
{
	if (0 != ec)
	{
		return;
	}

	std::shared_ptr<TimerEntry> entry = this->entry;
	{
		std::lock_guard<std::mutex> lo(lock);
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
	{
		std::lock_guard<std::mutex> lo(lock);
		if (nullptr == entry)
		{
			return false;
		}
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

std::string FromUnixtime(time_t now)
{
	if(0 == now)
	{
		return "0000-00-00 00:00:00";
	}
	struct tm when;
	char buf[20] = { 0 };
#ifdef _WIN32 // build Static multithreaded library "libcmt"
	gmtime_s(&when, &now);
	_snprintf_s(buf, 20, 19, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#else
	gmtime_r(&now, &when);
	snprintf(buf, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#endif
	return buf;
}

DateTime::DateTime()
{
}

DateTime::DateTime(const std::string& date)
{
	if(19 != date.length())
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidDateTimeFormat, date);
	}
	year = std::stoi(date.substr(0, 4));
	month = std::stoi(date.substr(5, 2));
	day = std::stoi(date.substr(8, 2));
	hour = std::stoi(date.substr(11, 2));
	minute = std::stoi(date.substr(14, 2));
	second = std::stoi(date.substr(17, 2));
}
}}