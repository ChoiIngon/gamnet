#ifndef _COMPONENT_COUNTER_H
#define _COMPONENT_COUNTER_H

#include <memory>
#include <Gamnet/Library/Time/Time.h>
#include <idl/MessageCommon.h>

class UserSession;

namespace Component {

class Counter
{
public :
	class Data
	{
	public:
		const uint64_t seq;
		const Message::CounterType type;
		Gamnet::Time::DateTime update_date;
	public:
		Data(const std::shared_ptr<UserSession>& session, uint64_t counter_seq, Message::CounterType counter_type, int count, const Gamnet::Time::DateTime& updateDate);
		virtual int Increase(int amount);
		virtual int Count();
	private:
		std::shared_ptr<UserSession> session;
		int count;
	};

public :
	Counter(const std::shared_ptr<UserSession>& session);
	void Load();
	std::shared_ptr<Data> AddCounter(const std::shared_ptr<Data>& counter);
	std::shared_ptr<Data> GetCounter(Message::CounterType type);
private :
	std::shared_ptr<UserSession> session;
	std::map<Message::CounterType, std::shared_ptr<Data>> counters;
};

}
#endif
