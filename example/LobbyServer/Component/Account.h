#ifndef _COMPONENT_ACCOUNT_H_
#define _COMPONENT_ACCOUNT_H_

#include "../../idl/MessageCommon.h"

namespace Component {
	class Account
	{
	public:
		Account(const std::string& accountID, Message::AccountType accountType, int account_level);

		const std::string account_id;
		const Message::AccountType account_type;
		const int account_level;

		Message::AccountState account_state;
		std::string user_name;
	};
};
#endif
