#ifndef _GAMNET_LIB_TIME_DATE_TIME_H_
#define _GAMNET_LIB_TIME_DATE_TIME_H_

#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace Gamnet { namespace Time {

class DateTime
{
private:
	boost::posix_time::ptime ptime;
public:
	static const DateTime MaxValue;
	static const DateTime MinValue;

	typedef boost::gregorian::days Days;
	typedef boost::posix_time::hours Hours;
	typedef boost::posix_time::minutes Minutes;
	typedef boost::posix_time::seconds Seconds;
	typedef boost::posix_time::time_duration Duration;

	DateTime();
	DateTime(time_t unixTimestamp);
	DateTime(int year, int month, int day, int hour, int minute, int second);
	DateTime(const std::string& fmt);
	DateTime(const boost::posix_time::ptime& ptime);

	DateTime& operator = (const std::string& fmt);
	DateTime& operator = (const boost::posix_time::ptime& ptime);

	boost::posix_time::ptime operator + (const boost::posix_time::time_duration& duration);
	boost::posix_time::ptime operator -	(const boost::posix_time::time_duration& duration);
	boost::posix_time::ptime& operator += (const boost::posix_time::time_duration& duration);
	boost::posix_time::ptime& operator -= (const boost::posix_time::time_duration& duration);

	bool operator == (const DateTime& rhs) const;
	bool operator != (const DateTime& rhs) const;
	bool operator > (const DateTime& rhs) const;
	bool operator < (const DateTime& rhs) const;
	bool operator >= (const DateTime& rhs) const;
	bool operator <= (const DateTime& rhs) const;

	int Year() const;
	int Month() const;
	int Day() const;
	int Hour() const;
	int Minute() const;
	int Second() const;

	std::string ToString() const;
	time_t ToUnixTimestamp() const;

	int DateDiff(const DateTime& rhs) const;
	int TimestampDiff(const DateTime& rhs) const;
private :
	void FromString(const std::string& fmt);
};

}}

std::ostream& operator << (std::ostream& os, const Gamnet::Time::DateTime& datetime);

#endif