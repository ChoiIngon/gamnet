#ifndef __GAMNET_LIB_TIME_H_
#define __GAMNET_LIB_TIME_H_

#include "DateTime.h"
#include "Timer.h"
namespace Gamnet { namespace Time {

	namespace UTC {
		time_t Now();
		DateTime ToUTC(const DateTime& datetime);
	}

	namespace Local {
		time_t Now();
	}

	time_t UnixTimestamp(const DateTime& datetime);
	DateTime FromUnixtime(time_t timestamp);
	int DateDiff(const DateTime& lhs, const DateTime& rhs);
	int TimestampDiff(const DateTime& lhs, const DateTime& rhs);
}}

#endif /* TIME_H_ */
