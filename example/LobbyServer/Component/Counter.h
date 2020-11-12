#ifndef _COMPONENT_COUNTER_H
#define _COMPONENT_COUNTER_H

#include <memory>
#include <Gamnet/Library/Time/Time.h>
#include <idl/MessageCommon.h>

class UserSession;

namespace Component {

class Counter
{
	class Data
	{
	public:
		Data(const std::shared_ptr<UserSession>& session, Message::CounterType counter_type, int count, const Gamnet::Time::DateTime& updateDate);
		virtual int Change(int amount);
		virtual int Count() const;
	private:
		std::shared_ptr<UserSession> session;
	public:
		const Message::CounterType type;
	private:
		int value;
	public:
		Gamnet::Time::DateTime update_date;
	};

public :
	Counter(const std::shared_ptr<UserSession>& session);
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
