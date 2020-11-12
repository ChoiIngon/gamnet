#include "Counter.h"
#include <idl/MessageUser.h>
#include "../UserSession.h"

namespace Component {

Counter::Data::Data(const std::shared_ptr<UserSession>& session, Message::CounterType counter_type, int count, const Gamnet::Time::DateTime& updateDate)
	: session(session)
	, type(counter_type)
	, value(count)
	, update_date(updateDate)
{
}

int Counter::Data::Change(int amount)
{
	this->value += amount;
	this->update_date = Gamnet::Time::FromUnixtime(Gamnet::Time::Local::Now());
	return value;
}

int Counter::Data::Count() const
{
	return value;
}

Counter::Counter(const std::shared_ptr<UserSession>& session)
	: session(session)
{
}

void Counter::Load()
{
	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
		"SELECT counter_type, counter_value, update_date FROM user_counter WHERE user_seq=", session->user_seq
	);

	Message::User::MsgSvrCli_Counter_Ntf ntf;
	for (auto& row : rows)
	{
		std::shared_ptr<Counter::Data> counter(std::make_shared<Counter::Data>(
			session, 
			(Message::CounterType)row->getUInt32("counter_type"), 
			row->getInt("counter_value"), 
			row->getString("update_date")
		));

		AddCounter(counter);
		
		Message::CounterData counterData;
		counterData.counter_type = counter->type;
		counterData.counter_value = counter->Count();
		counterData.update_date = Gamnet::Time::UnixTimestamp(counter->update_date);
		ntf.counter_datas.push_back(counterData);
	}
	if(0 < ntf.counter_datas.size())
	{
		Gamnet::Network::Tcp::SendMsg(session, ntf, true);
	}
}

std::shared_ptr<Counter::Data> Counter::AddCounter(const std::shared_ptr<Counter::Data>& counter)
{
	auto itr = counters.find(counter->type);
	if (counters.end() != itr)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
	}
	counters.insert(std::make_pair(counter->type, counter));
	return counter;
}

int Counter::GetCount(Message::CounterType type)
{
	auto itr = counters.find(type);
	if (counters.end() == itr)
	{
		return 0;
	}
		
	return itr->second->Count();
}

int Counter::ChangeCount(Message::CounterType type, int amount)
{
	auto itr = counters.find(type);
	if (counters.end() == itr)
	{
		session->queries->Execute("INSERT INTO user_counter(user_seq, counter_type, counter_value, update_date) VALUES(", session->user_seq, ",", (int)type, ",", amount, ", NOW())");
		std::shared_ptr<Counter::Data> counter(std::make_shared<Counter::Data>(
			session,
			(Message::CounterType)type,
			amount,
			Gamnet::Time::UTC::Now()
		));
		AddCounter(counter);
		return amount;
	}

	int counterValue = itr->second->Change(amount);
	session->queries->Execute("UPDATE user_counter SET counter_value=", counterValue, ", update_date=NOW() WHERE user_seq=", session->user_seq, " AND counter_type=", (int)type);
	session->on_commit.push_back([=]() {
		Message::User::MsgSvrCli_Counter_Ntf ntf;
		Message::CounterData counter;
		counter.counter_type = type;
		counter.counter_value = counterValue;
		ntf.counter_datas.push_back(counter);
		Gamnet::Network::Tcp::SendMsg(this->session, ntf, true);
	});
	return counterValue;
}

}


