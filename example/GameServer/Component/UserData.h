#ifndef _COMPONENT_USER_DATA_H_
#define _COMPONENT_USER_DATA_H_

#include <idl/MessageCommon.h>
#include <Gamnet/Library/Time/DateTime.h>
#include "UserData/Bag.h"
#include "UserData/Suit.h"
#include "UserData/Counter.h"

namespace Component {
	class UserData
	{
	public:
		UserData();

		std::string user_name;
		Gamnet::Time::DateTime offline_time;

		std::shared_ptr<Bag> pBag;
		std::shared_ptr<Suit> pSuit;

	};
};
#endif
