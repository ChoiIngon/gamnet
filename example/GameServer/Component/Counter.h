#ifndef _COMPONENT_COUNTER_H
#define _COMPONENT_COUNTER_H

#include <memory>
#include <Gamnet/Library/Time/Time.h>
#include <idl/MessageCommon.h>

class UserSession;

namespace Component {

class UserCounter
{
	typedef Gamnet::Time::DateTime DateTime;
	class Data
	{
	public:
		Data();
		virtual int Change(int amount, const DateTime& updateTime);
		virtual int Count() const;
	
	public:
		int64_t counter_no;
		Message::CounterType type;
		int value;
		Gamnet::Time::DateTime update_time;
	};

public :
	UserCounter();

	void Load();
	int GetCount(Message::CounterType type);
	int ChangeCount(Message::CounterType type, int amount);
private :
	std::shared_ptr<Data> AddCounter(const std::shared_ptr<Data>& counter);
	std::shared_ptr<UserSession> session;
	std::map<Message::CounterType, std::shared_ptr<Data>> counters;
};

}
#endif
