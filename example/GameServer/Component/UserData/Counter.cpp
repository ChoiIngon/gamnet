#include "Counter.h"
#include <idl/MessageUser.h>
#include "../../UserSession.h"
#include "../UserData.h"

namespace Component {

Counter::Data::Data()
	: type(Message::CounterType::Invalid)
	, value(0)
	, update_time(DateTime::MinValue)
{
}

void Counter::Serialize(std::list<Message::CounterData>& counter)
{
    for (auto& pair : datas)
    {
        Message::CounterData counterData;
        counterData.counter_type = pair.first;
        counterData.counter_value = pair.second->value;
		counterData.update_time = pair.second->update_time.ToUnixTimestamp();
        counter.push_back(counterData);
    }
}

void Counter::Load(const std::shared_ptr<UserSession>& session)
{
	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
		"SELECT counter_type, counter_value, update_time FROM UserCounter WHERE user_no=", session->user_no
	);

	auto pUserData = session->pUserData;
	auto pUserCounter = pUserData->pCounter;
	for (auto& row : rows)
	{
		std::shared_ptr<Data> data = std::make_shared<Data>();
		data->type = (Message::CounterType)row->getInt32("counter_type");
		data->value = row->getInt64("counter_value");
		data->update_time = row->getString("update_time");
		pUserCounter->datas.insert(std::make_pair(data->type, data));
	}
}

Gamnet::Return<std::shared_ptr<Transaction::Statement>> Counter::UpdateValue(const std::shared_ptr<UserSession>& session, Message::CounterType type, int amount)
{
	auto pUserData = session->pUserData;
	auto pUserCounter = pUserData->pCounter;

	std::shared_ptr<Transaction::Statement> statement = std::make_shared<Transaction::Statement>();

	std::shared_ptr<Counter::Data> data = nullptr;
    auto itr = pUserCounter->datas.find(type);
    if (pUserCounter->datas.end() == itr)
    {
        data = std::make_shared<Counter::Data>();
        data->type = type;
        data->value = amount;
		data->update_time = Gamnet::Time::Local::Now();
        pUserCounter->datas.insert(std::make_pair(data->type, data));

		statement->Commit += [data](const std::shared_ptr<UserSession>& session, const std::shared_ptr<Transaction::Connection>& db) {
			db->Execute(
				Gamnet::Format("INSERT INTO UserCounter(`user_no`, `counter_type`, `counter_value`, `update_time`) VALUES(", session->user_no, ",", (int)data->type, ",", data->value, ",'", data->update_time.ToString(), "')")
			);
		};
		statement->Rollback += [data](const std::shared_ptr<UserSession>& session) {
			std::shared_ptr<Component::UserData> pUserData = session->pUserData;
			std::shared_ptr<Component::Counter> pUserCounter = pUserData->pCounter;

			pUserCounter->datas.erase(data->type);
		};
    }
	else
	{
		data = itr->second;

		int64_t old_value = data->value;
		DateTime old_update_time = data->update_time;

		data->value = data->value + amount;
		data->update_time = Gamnet::Time::Local::Now();
		
		statement->Commit += [data](const std::shared_ptr<UserSession>& session, const std::shared_ptr<Transaction::Connection>& db) {
			db->Execute(
				Gamnet::Format("UPDATE UserCounter SET `counter_value` = ", data->value, ",`update_time`=", data->update_time.ToString(), " WHERE user_no=", session->user_no)
			);
		};

		statement->Rollback += [data, old_value, old_update_time](const std::shared_ptr<UserSession>& session) {
			std::shared_ptr<Component::UserData> pUserData = session->pUserData;
			std::shared_ptr<Component::Counter> pUserCounter = pUserData->pCounter;

			data->value = old_value;
			data->update_time = old_update_time;
		};
	}

	statement->Sync += [data](const std::shared_ptr<UserSession>& session) {
		Message::User::MsgSvrCli_Counter_Ntf ntf;
		Message::CounterData counterData;
		counterData.counter_type = data->type;
		counterData.counter_value = data->value;
		counterData.update_time = data->update_time.ToUnixTimestamp();
		ntf.counter_datas.push_back(counterData);
		Gamnet::Network::Tcp::SendMsg(session, ntf, true);
	};

    return statement;
}

}
void Test_Counter_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::User::MsgSvrCli_Counter_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	for (auto& counter : ntf.counter_datas)
	{
		session->counters.insert(std::make_pair(counter.counter_type, counter));
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::User::MsgSvrCli_Counter_Ntf, Test_Counter_Ntf
);