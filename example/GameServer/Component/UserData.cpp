#include "UserData.h"

namespace Component {
	UserData::UserData()
		: user_name()
		, offline_time(Gamnet::Time::DateTime::MinValue)
		, pBag(std::make_shared<Bag>())
		, pSuit(std::make_shared<Suit>())
		, pCounter(std::make_shared<Counter>())
	{
	}
}