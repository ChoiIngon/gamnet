#ifndef _COMPONENT_COUNTER_H
#define _COMPONENT_COUNTER_H

#include <memory>
#include <Gamnet/Library/Time/Time.h>

class UserSession;

namespace Component {

class CounterData
{
public:
	const uint64_t counter_seq;
	const uint32_t counter_id;
	Gamnet::Time::DateTime update_date;
public:
	CounterData(const std::shared_ptr<UserSession>& session, uint64_t counter_seq, uint32_t counter_id, int count, const Gamnet::Time::DateTime& updateDate);
	virtual int Increase(int amount);
	virtual int Count();
private:
	std::shared_ptr<UserSession> session;
	int count;
};

class Counter
{
public :
	Counter(const std::shared_ptr<UserSession>& session);
	void Load();
	std::shared_ptr<Component::CounterData> AddCounter(const std::shared_ptr<Component::CounterData>& counter);
	std::shared_ptr<Component::CounterData> GetCounter(uint32_t counterID);
private :
	std::shared_ptr<UserSession> session;
	std::map<uint32_t, std::shared_ptr<Component::CounterData>> counters;
};

}
#endif
