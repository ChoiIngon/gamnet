#ifndef _COMPONENT_ACCOUNT_H_
#define _COMPONENT_ACCOUNT_H_

#include <idl/MessageCommon.h>
#include <Gamnet/Library/Time/Time.h>
namespace Component {
	class Account
	{
	public:
		Account();

		int64_t user_no;
		std::string account_id;
		Message::AccountType account_type;
		int account_level;
		Message::AccountState account_state;
		int shard_index;
		Gamnet::Time::DateTime create_time;
		Gamnet::Time::DateTime delete_time;
	};
};
#endif
