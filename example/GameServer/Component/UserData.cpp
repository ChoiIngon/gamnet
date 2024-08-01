#include "UserData.h"

namespace Component {

UserData::UserData()
	: user_no(0)
	, user_name()
	, offline_time(Gamnet::Time::DateTime::MinValue)
	, pBag(std::make_shared<Bag>())
	, pSuit(std::make_shared<Suit>())
	, pCounter(std::make_shared<Counter>())
{
}

void UserData::Serialize(Message::UserData& userData)
{
	userData.user_no = user_no;
    userData.user_name = user_name;
    pBag->Serialize(userData.bag);
    pSuit->Serialize(userData.suit);
    pCounter->Serialize(userData.counter);
}

}