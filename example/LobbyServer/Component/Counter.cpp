#include "Counter.h"
#include <idl/MessageUser.h>
#include "../UserSession.h"

namespace Component {

CounterData::CounterData(const std::shared_ptr<UserSession>& session, uint64_t counterSEQ, uint32_t counterID, int count, const Gamnet::Time::DateTime& updateDate)
	: counter_seq(counterSEQ)
	, counter_id(counterID)
	, update_date(updateDate)
	, session(session)
	, count(count)
{
}

int CounterData::Increase(int amount)
{
	count += amount;
	update_date = Gamnet::Time::FromUnixtime(Gamnet::Time::Local::Now());
	session->queries->Execute("UPDATE user_counter SET counter=", count, ", update_date=NOW() WHERE counter_seq=", counter_seq);
	return count;
}

int CounterData::Count()
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
		"SELECT counter_seq, counter_id, counter, update_date FROM user_counter WHERE user_seq=", session->user_seq
	);

	if (0 == rows.GetRowCount())
	{
		session->queries->Insert("user_counter", {
			{ "counter_id", (int)Message::CounterType::Gold },
			{ "user_seq", session->user_seq }
			});

		session->queries->Insert("user_counter", {
			{ "counter_id", (int)Message::CounterType::Cash },
			{ "user_seq", session->user_seq }
			});

		session->queries->Commit();

		rows = Gamnet::Database::MySQL::Execute(session->shard_index,
			"SELECT counter_seq, counter_id, counter, update_date FROM user_counter WHERE user_seq=", session->user_seq
		);
	}

	if (0 == rows.GetRowCount())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	Message::User::MsgSvrCli_Counter_Ntf ntf;
	for (auto& row : rows)
	{
		std::shared_ptr<Component::CounterData> counter(std::make_shared<Component::CounterData>(session, row->getUInt64("counter_seq"), row->getUInt32("counter_id"), row->getInt("counter"), row->getString("update_date")));
		AddCounter(counter);
		
		Message::CounterData counterData;
		counterData.counter_id = (Message::CounterType)counter->counter_id;
		counterData.update_date = Gamnet::Time::UnixTimestamp(counter->update_date);
		counterData.count = counter->Count();
		ntf.counter_datas.push_back(counterData);
	}
	if(0 < ntf.counter_datas.size())
	{
		Gamnet::Network::Tcp::SendMsg(session, ntf, true);
	}
}

std::shared_ptr<Component::CounterData> Counter::AddCounter(const std::shared_ptr<Component::CounterData>& counter)
{
	auto itr = counters.find(counter->counter_id);
	if (counters.end() != itr)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
	}
	counters.insert(std::make_pair(counter->counter_id, counter));
	return counter;
}

std::shared_ptr<Component::CounterData> Counter::GetCounter(uint32_t counterID)
{
	auto itr = counters.find(counterID);
	if (counters.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}


}