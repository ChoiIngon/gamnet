#include "UserData.h"

namespace Component {
	UserData::UserData()
		: user_name()
		, offline_time(Gamnet::Time::DateTime::MinValue)
	{
	}
}