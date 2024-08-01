#ifndef _COMPONENT_COUNTER_H
#define _COMPONENT_COUNTER_H

#include <memory>
#include <Gamnet/Library/Time/Time.h>
#include <idl/MessageCommon.h>

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

	void Serialize(std::list<Message::CounterData>& counter);
private :
	std::map<Message::CounterType, std::shared_ptr<Data>> datas;
};

}
#endif
