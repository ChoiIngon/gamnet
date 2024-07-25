#ifndef _COMPONENT_USER_DATA_H_
#define _COMPONENT_USER_DATA_H_

#include <idl/MessageCommon.h>
#include <Gamnet/Library/Time/DateTime.h>

namespace Component {
	class UserData
	{
	public:
		UserData();

		std::string user_name;
		Gamnet::Time::DateTime offline_time;
	};
};
#endif
