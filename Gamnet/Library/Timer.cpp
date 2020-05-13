#include "Timer.h"

namespace Gamnet { namespace Time {

Timer::Timer()
	: entry_(nullptr), interval_(0), auto_reset_(false), deadline_timer_(Singleton<boost::asio::io_service>::GetInstance())
{
}

Timer::Timer(boost::asio::io_service& ioService) :
	entry_(nullptr), interval_(0), auto_reset_(false), deadline_timer_(ioService)
{
}

Timer::~Timer()
{
	deadline_timer_.expires_at(boost::posix_time::pos_infin);
	entry_ = nullptr;
}

void Timer::OnExpire(const boost::system::error_code& ec)
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

	if (true == auto_reset_)
	{
		Resume();
	}
}

bool Timer::Resume()
{
	{
		std::lock_guard<std::mutex> lo(lock_);
		if (nullptr == entry_)
		{
			return false;
		}
	}
	deadline_timer_.expires_at(deadline_timer_.expires_at() + boost::posix_time::milliseconds(interval_));
	deadline_timer_.async_wait(boost::bind(&Timer::OnExpire, this, boost::asio::placeholders::error));
	return true;
}

void Timer::AutoReset(bool flag)
{
	auto_reset_ = flag;
}

void Timer::Cancel()
{
	deadline_timer_.cancel();
	std::lock_guard<std::mutex> lo(lock_);
	entry_ = nullptr;
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
}}