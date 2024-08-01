#ifndef _COMPONENT_COUNTER_H
#define _COMPONENT_COUNTER_H

#include <memory>
#include <Gamnet/Library/Time/Time.h>
#include <idl/MessageCommon.h>
#include <Gamnet/Library/Return.h>
#include <Gamnet/Library/Time/Time.h>
#include "../../Util/Transaction.h"

class UserSession;

namespace Component {

class Counter
{
	typedef Message::CounterType Type;
	typedef Gamnet::Time::DateTime DateTime;

	class Data
	{
	public:
		Data();

		Type		type;
		int64_t		value;
		DateTime	update_time;
	};

public :
	Counter() = default;

	static void Load(const std::shared_ptr<UserSession>& session);
	static Gamnet::Return<std::shared_ptr<Transaction::Statement>> UpdateValue(const std::shared_ptr<UserSession>& session, Type type, int amount);
	
	void Serialize(std::list<Message::CounterData>& counter);
private :
	std::map<Message::CounterType, std::shared_ptr<Data>> datas;
};

}
#endif
