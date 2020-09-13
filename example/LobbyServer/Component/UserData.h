#ifndef _COMPONENT_USER_DATA_H_
#define _COMPONENT_USER_DATA_H_

#include <idl/MessageCommon.h>
#include <Gamnet/Library/Time/DateTime.h>

namespace Component {
	class UserData
	{
	public:
		UserData(uint64_t userSEQ, const std::string& name);

		const uint64_t user_seq;
		const std::string user_name;
		Gamnet::Time::DateTime create_date;
		Gamnet::Time::DateTime offline_date;
	};
};
#endif
