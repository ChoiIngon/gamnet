#include "Account.h"

namespace Component {
	Account::Account(const std::string& accountID, Message::AccountType accountType, int accountLevel)
		: account_id(accountID)
		, account_type(accountType)
		, account_level(accountLevel)
		, account_state(Message::AccountState::Invalid)
		, user_name("")
	{
	}
}