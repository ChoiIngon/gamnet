#include "DateTime.h"
#include "../Exception.h"
#include <boost/date_time/posix_time/conversion.hpp>

namespace Gamnet { namespace Time {

const DateTime DateTime::MaxValue("2038-01-19 03:14:07");
const DateTime DateTime::MinValue;

DateTime::DateTime() : ptime(boost::posix_time::ptime(boost::posix_time::min_date_time))
{
}

DateTime::DateTime(time_t timestamp) : ptime(boost::posix_time::from_time_t(timestamp))
{
}

DateTime::DateTime(const std::string& fmtDate)
{
	FromString(fmtDate);
}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second)
{
	ptime = boost::posix_time::ptime(boost::gregorian::date(year, month, day), boost::posix_time::time_duration(hour, minute, second, 0));
}

DateTime::DateTime(const boost::posix_time::ptime& ptime) : ptime(ptime)
{
}

int DateTime::Year() const 
{
	const boost::gregorian::date& date = ptime.date();
	return date.year();
}

int DateTime::Month() const
{
	const boost::gregorian::date& date = ptime.date();
	return date.month();
}

int DateTime::Day() const
{
	const boost::gregorian::date& date = ptime.date();
	return date.day();
}

int DateTime::Hour() const
{
	const boost::posix_time::time_duration& time = ptime.time_of_day();
	return time.hours();
}

int DateTime::Minute() const
{
	const boost::posix_time::time_duration& time = ptime.time_of_day();
	return time.minutes();
}

int DateTime::Second() const
{
	const boost::posix_time::time_duration& time = ptime.time_of_day();
	return time.seconds();
}

void DateTime::FromString(const std::string& fmt)
{
	try {
		ptime = boost::posix_time::ptime(boost::posix_time::time_from_string(fmt));
	}
	catch (const std::out_of_range& e)
	{
		ptime = boost::posix_time::min_date_time;
	}
}

std::string DateTime::ToString() const
{
	const boost::gregorian::date& date = ptime.date();
	return boost::gregorian::to_iso_extended_string(date) + " " + boost::posix_time::to_simple_string(ptime).substr(12, 8);
}

time_t DateTime::ToUnixTimestamp() const
{
	return boost::posix_time::to_time_t(ptime);
}

DateTime& DateTime::operator = (const std::string& fmt)
{
	FromString(fmt);
	return *this;
}
DateTime& DateTime::operator = (const boost::posix_time::ptime& ptime)
{
	this->ptime = ptime;
	return *this;
}
boost::posix_time::ptime DateTime::operator + (const boost::posix_time::time_duration& duration)
{
	return this->ptime + duration;
}

boost::posix_time::ptime DateTime::operator - (const boost::posix_time::time_duration& duration)
{
	return this->ptime - duration;
}

boost::posix_time::ptime& DateTime::operator += (const boost::posix_time::time_duration& duration)
{
	return this->ptime = this->ptime + duration;
}

boost::posix_time::ptime& DateTime::operator -= (const boost::posix_time::time_duration& duration)
{
	return this->ptime = this->ptime - duration;
}

bool DateTime::operator == (const DateTime& rhs) const
{
	return this->ptime == rhs.ptime;
}

bool DateTime::operator != (const DateTime& rhs) const
{
	return this->ptime != rhs.ptime;
}
bool DateTime::operator > (const DateTime& rhs) const
{
	return this->ptime > rhs.ptime;
}
bool DateTime::operator < (const DateTime& rhs) const
{
	return this->ptime < rhs.ptime;
}
bool DateTime::operator >= (const DateTime& rhs) const
{
	return this->ptime >= rhs.ptime;
}
bool DateTime::operator <= (const DateTime& rhs) const
{
	return this->ptime <= rhs.ptime;
}

int DateTime::DateDiff(const DateTime& rhs) const
{
	boost::gregorian::days duration = this->ptime.date() - rhs.ptime.date();
	return duration.days();
}

int DateTime::TimestampDiff(const DateTime& rhs) const
{
	boost::posix_time::time_duration duration = this->ptime - rhs.ptime;
	return duration.total_seconds();
}

}}

std::ostream& operator << (std::ostream& os, const Gamnet::Time::DateTime& datetime)
{
	return os << datetime.ToString();
}
