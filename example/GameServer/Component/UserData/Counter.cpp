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

}


