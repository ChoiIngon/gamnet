#include "UserData.h"

namespace Component {
	UserData::UserData(uint64_t userSEQ, const std::string& name)
		: user_seq(userSEQ)
		, user_name(name)
		, create_date(Gamnet::Time::DateTime::MinValue)
		, offline_date(Gamnet::Time::DateTime::MinValue)
	{
	}
}