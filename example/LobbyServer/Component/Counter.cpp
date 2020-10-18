#include "Counter.h"
#include <idl/MessageUser.h>
#include "../UserSession.h"

namespace Component {

Counter::Data::Data(const std::shared_ptr<UserSession>& session, uint64_t seq, Message::CounterType counter_type, int count, const Gamnet::Time::DateTime& updateDate)
	: seq(seq)
	, type(counter_type)
	, update_date(updateDate)
	, session(session)
	, count(count)
{
}

int Counter::Data::Increase(int amount)
{
	count += amount;
	update_date = Gamnet::Time::FromUnixtime(Gamnet::Time::Local::Now());
	session->queries->Execute("UPDATE user_counter SET counter=", count, ", update_date=NOW() WHERE counter_seq=", seq);
	return count;
}

int Counter::Data::Count()
{
	return count;
}

Counter::Counter(const std::shared_ptr<UserSession>& session)
	: session(session)
{
}

void Counter::Load()
{
	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
		"SELECT counter_seq, counter_type, counter, update_date FROM user_counter WHERE user_seq=", session->user_seq
	);

	if (0 == rows.GetRowCount())
	{
		session->queries->Insert("user_counter", {
			{ "counter_type", (int)Message::CounterType::Gold },
			{ "user_seq", session->user_seq }
			});

		session->queries->Insert("user_counter", {
			{ "counter_type", (int)Message::CounterType::Cash },
			{ "user_seq", session->user_seq }
			});

		session->queries->Commit();

		rows = Gamnet::Database::MySQL::Execute(session->shard_index,
			"SELECT counter_seq, counter_type, counter, update_date FROM user_counter WHERE user_seq=", session->user_seq
		);
	}

	if (0 == rows.GetRowCount())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	Message::User::MsgSvrCli_Counter_Ntf ntf;
	for (auto& row : rows)
	{
		std::shared_ptr<Counter::Data> counter(std::make_shared<Counter::Data>(session, row->getUInt64("counter_seq"), (Message::CounterType)row->getUInt32("counter_type"), row->getInt("counter"), row->getString("update_date")));
		AddCounter(counter);
		
		Message::CounterData counterData;
		counterData.counter_type = counter->type;
		counterData.update_date = Gamnet::Time::UnixTimestamp(counter->update_date);
		counterData.count = counter->Count();
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

std::shared_ptr<Counter::Data> Counter::GetCounter(Message::CounterType type)
{
	auto itr = counters.find(type);
	if (counters.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}


}