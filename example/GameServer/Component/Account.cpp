#include "Account.h"

namespace Component {
	Account::Account()
		: user_no(0)
		, account_id("")
		, account_type(Message::AccountType::Invalid)
		, account_level(0)
		, account_state(Message::AccountState::Invalid)
	{
	}
}