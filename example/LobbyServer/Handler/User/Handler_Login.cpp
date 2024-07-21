#include "Handler_Login.h"
#include "../../Component/Counter.h"
#include "../../Component/Account.h"
#include "../../Component/UserData.h"
#include "../../Component/Item.h"
#include "../../Component/Bag.h"
#include "../../Component/Suit.h"
#include "../../../idl/MessageItem.h"

namespace Handler { namespace User {

Handler_Login::Handler_Login()
{
}

Handler_Login::~Handler_Login()
{
}

void Handler_Login::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::User::MsgCliSvr_Login_Req& req)
{
	Message::User::MsgSvrCli_Login_Ans ans;
	ans.error_code = Message::ErrorCode::Success;
	ans.user_data.user_seq = 0;
	ans.user_data.user_name = "";

	try {
		LOG(DEV, "Message::User::MsgCliSvr_Login_Req(account_id:", req.account_id, ")");
		ReadAccountData(session, req.account_id, req.account_type);
		ReadUserData(session);
		ReadUserCounter(session);
		ReadUserItem(session);
		
		std::shared_ptr<Component::UserData> userData = session->GetComponent<Component::UserData>();
		ans.user_data.user_seq = userData->user_seq;
		ans.user_data.user_name = userData->user_name;
	}
	catch (const Gamnet::Exception& e)
	{
		ans.error_code = (Message::ErrorCode)e.error_code();
		if(Message::ErrorCode::InvalidUserError != ans.error_code)
		{
			LOG(
				, e.what());
		}
		session->RemoveComponent<Component::Account>();
		session->user_seq = 0;
		session->shard_index = 0;
	}
	LOG(DEV, "Message::User::MsgSvrCli_Login_Ans(error_code:", (int)ans.error_code, ", user_seq:", ans.user_data.user_seq, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::User::MsgCliSvr_Login_Req,
	Handler_Login, Recv_Req,
	HandlerCreate
);

void Handler_Login::ReadAccountData(const std::shared_ptr<UserSession>& session, const std::string& accountID, Message::AccountType accountType)
{
	if(nullptr != session->GetComponent<Component::Account>())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
		"SELECT user_seq, user_name, account_level, account_state, shard_index, penalty_date, delete_date FROM account WHERE account_id='", accountID, "' and account_type=", (int)accountType
	);
	if (1 != rows.GetRowCount())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError, "account_id:", accountID, ", account_type:", (int)accountType);
	}

	auto row = rows[0];

	session->shard_index = row->getInt("shard_index");
	session->user_seq = row->getUInt64("user_seq");
	session->queries = std::make_shared<Gamnet::Database::MySQL::Transaction>(session->shard_index);
	session->logs = std::make_shared<Gamnet::Database::MySQL::Transaction>(session->shard_index);

	std::shared_ptr<Component::Account> account = session->AddComponent<Component::Account>(std::make_shared<Component::Account>(accountID, accountType, row->getInt("account_level")));
	account->account_state = (Message::AccountState)row->getInt("account_state");
	account->user_name = row->getString("user_name");

	time_t now = Gamnet::Time::UTC::Now();
	if (Message::AccountState::OnDelete == account->account_state &&  now < Gamnet::Time::UnixTimestamp(Gamnet::Time::DateTime(row->getString("delete_date"))))
	{
		Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
			"UPDATE account SET account_id='", account->account_id, "-DEL-", session->user_seq, "',account_state=4 WHERE user_seq=", session->user_seq
		);

		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError, "account_id:", account->account_id, ", account_type:", (int)account->account_type, ", deleted");
	}

	if (Message::AccountState::Penalty == account->account_state && now < Gamnet::Time::UnixTimestamp(Gamnet::Time::DateTime(row->getString("penalty_date"))))
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "account_id:", account->account_id, ", account_type:", (int)account->account_type, ", deleted");
	}
}

void Handler_Login::ReadUserData(const std::shared_ptr<UserSession>& session)
{
	auto account = session->GetComponent<Component::Account>();
	if(nullptr == account)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	uint32_t localServerID = Gamnet::Network::Tcp::GetLocalAddress().to_v4().to_uint();
	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
		"SELECT user_name, server_id, create_date, offline_date FROM user_data WHERE user_seq=", session->user_seq
	);

	if (1 != rows.GetRowCount())
	{
		Gamnet::Database::MySQL::Execute(session->shard_index,
			"INSERT INTO user_data(user_seq, user_name, server_id) VALUES (", session->user_seq, ",'", account->user_name, "',", localServerID, ")"
		);
		rows = Gamnet::Database::MySQL::Execute(session->shard_index,
			"SELECT user_name, server_id, create_date, offline_date FROM user_data WHERE user_seq=", session->user_seq
		);
	}

	if (1 != rows.GetRowCount())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError, "account_id:", account->account_id, ", account_type:", (int)account->account_type);
	}

	std::shared_ptr<UserSession> prev = Gamnet::Singleton<UserSession::Manager>::GetInstance().AddSession(session);
	if(nullptr != prev)
	{
		prev->handover_safe = false;
		prev->Close(0);
	}
	auto row = rows[0];
	if (localServerID == row->getUInt("server_id"))
	{
		
	}

	std::shared_ptr<Component::UserData> userData = session->AddComponent<Component::UserData>(std::make_shared<Component::UserData>(session->user_seq, account->user_name));
}

void Handler_Login::ReadUserCounter(const std::shared_ptr<UserSession>& session)
{
	if (nullptr == session->GetComponent<Component::UserData>())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}
	std::shared_ptr<Component::Counter> counter = session->GetComponent<Component::Counter>();
	assert(nullptr != counter);
	counter->Load();
}

void Handler_Login::ReadUserItem(const std::shared_ptr<UserSession>& session)
{
	std::shared_ptr<Component::Bag> bag = session->GetComponent<Component::Bag>();
	bag->Load();
}

void Test_Login_Req(const std::shared_ptr<TestSession>& session)
{
	Message::User::MsgCliSvr_Login_Req req;
	req.account_id = session->session_token;
	req.account_type = Message::AccountType::Dev;
	Gamnet::Test::SendMsg(session, req);
}

void Test_Login_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::User::MsgSvrCli_Login_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserLogin_Ans");
		if(Message::ErrorCode::InvalidUserError == ans.error_code)
		{
			Message::User::MsgCliSvr_Create_Req req;
			req.user_name = session->session_token.substr(0, 16);
			req.account_id = session->session_token;
			req.account_type = Message::AccountType::Dev;
			Gamnet::Test::SendMsg(session, req);
			return;
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_User_Login",
	Message::User::MsgCliSvr_Login_Req, Test_Login_Req,
	Message::User::MsgSvrCli_Login_Ans, Test_Login_Ans
);

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
		LOG(ERR, e.what());
	}

	for(auto& counter : ntf.counter_datas)
	{
		session->counters.insert(std::make_pair(counter.counter_type, counter));
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::User::MsgSvrCli_Counter_Ntf, Test_Counter_Ntf
);

}}

