#include "Counter.h"
#include <idl/MessageUser.h>
#include "../UserSession.h"

namespace Component {

UserCounter::Data::Data(const std::shared_ptr<UserSession>& session, Message::CounterType counter_type, int count, const Gamnet::Time::DateTime& updateDate)
	: session(session)
	, type(counter_type)
	, value(count)
	, update_date(updateDate)
{
}

int UserCounter::Data::Change(int amount)
{
	this->value += amount;
	this->update_date = Gamnet::Time::FromUnixtime(Gamnet::Time::Local::Now());
	return value;
}

int UserCounter::Data::Count() const
{
	return value;
}

UserCounter::UserCounter(const std::shared_ptr<UserSession>& session)
	: session(session)
{
}

void UserCounter::Load()
{
	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
		"SELECT counter_no, counter_type, counter_value, update_date FROM UserCounter WHERE user_no=", session->user_no
	);

	Message::User::MsgSvrCli_Counter_Ntf ntf;
	for (auto& row : rows)
	{
		std::shared_ptr<Data> data = std::make_shared<Data>();
		data->counter_no = row->getInt64("counter_no");
		data->type = (Message::CounterType)row->getUInt32("counter_type");
		data->value = row->getInt("counter_value");
		data->update_time = row->getString("update_time");

		AddCounter(data);
		
		Message::CounterData counterData;
		counterData.counter_type = data->type;
		counterData.counter_value = data->Count();
		counterData.update_date = Gamnet::Time::UnixTimestamp(data->update_time);
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
		session->queries->Execute("INSERT INTO user_counter(user_seq, counter_type, counter_value, update_date) VALUES(", session->user_no, ",", (int)type, ",", amount, ", NOW())");
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
	session->queries->Execute("UPDATE user_counter SET counter_value=", counterValue, ", update_date=NOW() WHERE user_seq=", session->user_no, " AND counter_type=", (int)type);
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


