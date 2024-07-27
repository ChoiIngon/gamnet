#include "Handler_Login.h"
#include "../../Component/UserCounter.h"
#include "../../Component/Account.h"
#include "../../Component/UserData.h"
#include "../../Component/Item.h"
#include "../../Component/Bag.h"
#include "../../Component/Suit.h"
#include "../../../idl/MessageItem.h"
#include "../../../idl/MessageCommon.h"
#include <future>
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
		if (0 != session->user_no)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		ReadAccountData(session, req.account_id, req.account_type);
		ReadUserData(session);

		auto asyncUserCounter = std::async(std::launch::async, &Component::UserCounter::Load, session);
		auto asyncUserBag = std::async(std::launch::async, &Component::Bag::Load, session);
		
		session->AddComponent(asyncUserCounter.get());
		session->AddComponent(asyncUserBag.get());

		std::shared_ptr<Component::Account> pAccount = session->GetComponent<Component::Account>();
		std::shared_ptr<Component::UserData> pUserData = session->GetComponent<Component::UserData>();
		
		ans.user_data.user_seq = pAccount->user_no;
		ans.user_data.user_name = pUserData->user_name;
	}
	catch (const Gamnet::Exception& e)
	{
		ans.error_code = (Message::ErrorCode)e.error_code();
		if(Message::ErrorCode::InvalidUserError != ans.error_code)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		}
		session->RemoveComponent<Component::Account>();
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
		"SELECT user_no, account_level, account_state, shard_index, create_time, delete_time FROM Account WHERE account_id='", accountID, "' and account_type=", (int)accountType
	);

	if (1 != rows.GetRowCount())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError, "account_id:", accountID, ", account_type:", (int)accountType);
	}

	auto row = rows[0];
	session->user_no = row->getInt64("user_no");
	session->shard_index = row->getInt("shard_index");

	std::shared_ptr<Component::Account> account = std::make_shared<Component::Account>();
	account->user_no = row->getInt64("user_no");
	account->account_id = accountID;
	account->account_type = accountType;
	account->account_state = (Message::AccountState)row->getInt("account_state");
	account->shard_index = row->getInt("shard_index");
	account->create_time = row->getString("create_time");
	account->delete_time = row->getString("delete_time");

	time_t now = Gamnet::Time::UTC::Now();
	if (Message::AccountState::OnDelete == account->account_state &&  now < Gamnet::Time::UnixTimestamp(account->delete_time))
	{
		Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
			"UPDATE account SET account_id='", account->account_id, "-DEL-", session->user_no, "',account_state=4 WHERE user_seq=", account->user_no
		);

		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError, "account_id:", account->account_id, ", account_type:", (int)account->account_type, ", deleted");
	}

	session->AddComponent(account);
}

void Handler_Login::ReadUserData(const std::shared_ptr<UserSession>& session)
{
	auto account = session->GetComponent<Component::Account>();
	if(nullptr == account)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}
	
	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
		"SELECT user_name, server_id, offline_time FROM UserData WHERE user_no=", session->user_no
	);

	if (1 != rows.GetRowCount())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError, "account_id:", account->account_id, ", account_type:", (int)account->account_type);
	}

	// 중복 세션 강제 종료
	std::shared_ptr<UserSession> prev = Gamnet::Singleton<UserSession::Manager>::GetInstance().AddSession(session);
	if (nullptr != prev)
	{
		prev->handover_safe = false;
		prev->Close(0);
	}

	auto row = rows[0];
	const uint32_t localServerID = Gamnet::Network::Tcp::GetLocalAddress().to_v4().to_uint();
	const uint32_t server_id = row->getUInt32("server_id");
	if (0 != server_id && server_id != localServerID)
	{
		Message::User::MsgSvrSvr_Kickout_Ntf ntf;
		Gamnet::Network::Router::Address dest;
		dest.cast_type = Gamnet::Network::Router::Address::CAST_TYPE::UNI_CAST;
		dest.service_name = "GAME";
		dest.id = server_id;
		Gamnet::Network::Router::SendMsg(dest, ntf);
	}

	std::shared_ptr<Component::UserData> pUserData = std::make_shared<Component::UserData>();
	pUserData->user_name = row->getString("user_name");
	pUserData->offline_time = row->getString("offline_time");

	session->AddComponent(pUserData);
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
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "MsgCliSvr_Login_Req",
	Message::User::MsgCliSvr_Login_Req, Test_Login_Req,
	Message::User::MsgSvrCli_Login_Ans, Test_Login_Ans
);



}}

