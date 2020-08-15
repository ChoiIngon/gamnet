#include "Handler_Login.h"
#include "../../Component/Counter.h"
#include "../../../idl/MessageUser.h"
#include "../../Component/Item.h"
#include "../../Component/Bag.h"

namespace Handler { namespace User {

Handler_Login::Handler_Login()
{
}

Handler_Login::~Handler_Login()
{
}

void Handler_Login::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_Login_Req req;
	MsgSvrCli_Login_Ans ans;
	ans.error_code = ErrorCode::Success;
	ans.user_data.user_seq = 0;
	ans.user_data.user_name = "";

	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_Login_Req(account_id:", req.account_id, ")");

		session->account_id = req.account_id;
		session->account_type = req.account_type;

		ReadUserData(session);
		ReadUserCounter(session);
		ReadUserItem(session);

		std::shared_ptr<UserData> userData = session->GetComponent<UserData>();
		ans.user_data = *userData;
		session->Commit();
	}
	catch (const Gamnet::Exception& e)
	{
		ans.error_code = (ErrorCode)e.error_code();
		if(ErrorCode::InvalidUserError != ans.error_code)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		}
	}
	LOG(DEV, "MsgSvrCli_Login_Ans(error_code:", (int)ans.error_code, ", user_seq:", ans.user_data.user_seq, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_Login_Req,
	Handler_Login, Recv_Req,
	HandlerCreate
);

void Handler_Login::ReadUserData(const std::shared_ptr<UserSession>& session)
{
	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
		"SELECT user_seq, user_name, account_level, account_state, shard_index, create_date, delete_date FROM account WHERE account_id='", session->account_id, "' and account_type=", (int)session->account_type
	);

	if (1 != rows.GetRowCount())
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError, "account_id:", session->account_id, ", account_type:", (int)session->account_type);
	}

	auto& row = rows[0];
	
	session->account_level = row->getInt("account_level");
	session->shard_index = row->getInt("shard_index");
	session->create_date = row->getString("create_date");
	session->queries = std::make_shared<Gamnet::Database::MySQL::Transaction>(session->shard_index);
	session->logs = std::make_shared<Gamnet::Database::MySQL::Transaction>(session->shard_index);
	session->user_seq = row->getUInt64("user_seq");

	if(3 == row->getInt("account_state") && Gamnet::Time::UTC::Now() < Gamnet::Time::UnixTimestamp(Gamnet::Time::DateTime(row->getString("delete_date"))))
	{
		Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
			"UPDATE account SET account_id='", session->account_id, "-DEL-", session->user_seq, "',account_state=4 WHERE user_seq=", session->user_seq
		);

		throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError, "account_id:", session->account_id, ", account_type:", (int)session->account_type, ", deleted");
	}
		
	std::shared_ptr<UserData> userData = session->AddComponent<UserData>();
	userData->user_seq = session->user_seq;
	userData->user_name = row->getString("user_name");
}

void Handler_Login::ReadUserCounter(const std::shared_ptr<UserSession>& session)
{
	std::shared_ptr<Component::Counter> counter = session->GetComponent<Component::Counter>();
	counter->Load();
}

void Handler_Login::ReadUserItem(const std::shared_ptr<UserSession>& session)
{
	std::shared_ptr<Component::Bag> bag = session->GetComponent<Component::Bag>();
	bag->Load();
}

void Test_Login_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_Login_Req req;
	req.account_id = session->session_token;
	req.account_type = AccountType::Dev;
	Gamnet::Test::SendMsg(session, req);
}

void Test_Login_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Login_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserLogin_Ans");
		if(ErrorCode::InvalidUserError == ans.error_code)
		{
			MsgCliSvr_Create_Req req;
			req.user_name = session->session_token.substr(0, 16);
			req.account_id = session->session_token;
			req.account_type = AccountType::Dev;
			Gamnet::Test::SendMsg(session, req);
			return;
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_User_Login",
	MsgCliSvr_Login_Req, Test_Login_Req,
	MsgSvrCli_Login_Ans, Test_Login_Ans
);

void Test_Counter_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Counter_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	for(auto& counter : ntf.counter_datas)
	{
		session->counters.insert(std::make_pair((int)counter.counter_id, counter));
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	MsgSvrCli_Counter_Ntf, Test_Counter_Ntf
);


}}