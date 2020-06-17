#include "Time.h"

namespace Gamnet { namespace Time {
	namespace UTC {
		time_t Now()
		{
			return boost::posix_time::to_time_t(boost::posix_time::second_clock::universal_time());
		}
	}

	namespace Local {
		time_t Now()
		{
			return boost::posix_time::to_time_t(boost::posix_time::second_clock::local_time());
		}
	}

	DateTime FromUnixtime(time_t timestamp)
	{
		return DateTime(timestamp);
	}

	int DateDiff(const DateTime& lhs, const DateTime& rhs)
	{
		return lhs.DateDiff(rhs);
	}

	int TimestampDiff(const DateTime& lhs, const DateTime& rhs)
	{
		return lhs.TimestampDiff(rhs);
	}
}}
