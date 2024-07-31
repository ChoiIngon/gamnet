#ifndef _COMPONENT_ACCOUNT_H_
#define _COMPONENT_ACCOUNT_H_

#include <idl/MessageCommon.h>
#include <Gamnet/Library/Time/Time.h>
namespace Component 
{
	class Account
	{
	public:
		typedef Gamnet::Time::DateTime DateTime;
		typedef Message::AccountType AccountType;
		typedef Message::AccountState AccountState;

		Account();

		std::string		account_id;
		int64_t			user_no;
		AccountType		account_type;
		AccountState	account_state;
		int				account_level;
		int				shard_index;
		DateTime		create_time;
		DateTime		delete_time;
	};
}

#endif