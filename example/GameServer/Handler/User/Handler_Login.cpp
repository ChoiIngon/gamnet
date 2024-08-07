#include "Handler_Login.h"
#include "../../Component/Account.h"
#include "../../Component/UserData.h"
#include "../../../idl/MessageItem.h"
#include "../../../idl/MessageCommon.h"
#include "../../Util/Transaction.h"

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
	ans.user_data.user_no = 0;
	ans.user_data.user_name = "";

	try {
		LOG(DEV, "Message::User::MsgCliSvr_Login_Req(account_id:", req.account_id, ")");
		if (0 != session->user_no)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		ReadAccountData(session, req.account_id, req.account_type);
		ReadUserData(session);

		std::shared_ptr<Component::UserData> pUserData = session->pUserData;
		pUserData->Serialize(ans.user_data);
	}
	catch (const Gamnet::Exception& e)
	{
		ans.error_code = (Message::ErrorCode)e.error_code();
		if(Message::ErrorCode::InvalidUserError != ans.error_code)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		}
	}
	LOG(DEV, "Message::User::MsgSvrCli_Login_Ans(error_code:", (int)ans.error_code, ", user_seq:", ans.user_data.user_no, ")");
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
	auto pAccount = session->pAccount;
	if(Message::AccountState::Invalid != pAccount->account_state)
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

	pAccount->user_no = row->getInt64("user_no");
	pAccount->account_id = accountID;
	pAccount->account_type = accountType;
	pAccount->account_state = (Message::AccountState)row->getInt("account_state");
	pAccount->shard_index = row->getInt("shard_index");
	pAccount->create_time = row->getString("create_time");
	pAccount->delete_time = row->getString("delete_time");

	time_t now = Gamnet::Time::UTC::Now();
	if (Message::AccountState::OnDelete == pAccount->account_state &&  now < Gamnet::Time::UnixTimestamp(pAccount->delete_time))
	{
		Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
			"UPDATE account SET account_id='", pAccount->account_id, "-DEL-", session->user_no, "',account_state=4 WHERE user_seq=", pAccount->user_no
		);

		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError, "account_id:", pAccount->account_id, ", account_type:", (int)pAccount->account_type, ", deleted");
	}
}

void Handler_Login::ReadUserData(const std::shared_ptr<UserSession>& session)
{
	auto pAccount = session->pAccount;
	if (Message::AccountState::Normal != pAccount->account_state)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}
	
	Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
		"SELECT user_name, server_id, offline_time FROM UserData WHERE user_no=", session->user_no
	);

	if (1 != rows.GetRowCount())
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError, "account_id:", pAccount->account_id, ", account_type:", (int)pAccount->account_type);
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

		Gamnet::Database::MySQL::Execute(session->shard_index,
			"UPDATE UserData SET server_id = ", localServerID, " WHERE user_no=", session->user_no
		);
	}

	auto pUserData = session->pUserData;
	pUserData->user_no = session->user_no;
	pUserData->user_name = row->getString("user_name");
	pUserData->offline_time = row->getString("offline_time");

	auto asyncUserCounter = std::async(std::launch::async, &Component::Counter::Load, session);
	auto asyncLoadItem = std::async(std::launch::async, &Item::Load, session);

	asyncUserCounter.wait();
	asyncLoadItem.wait();
}

void Test_Login_Req(const std::shared_ptr<TestSession>& session)
{
	Message::User::MsgCliSvr_Login_Req req;
	//req.account_id = "a8a21a2984c2be156471f538a9760a93";
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
		
		std::shared_ptr<Component::UserData> pUserData = std::make_shared<Component::UserData>();
		session->AddComponent(pUserData);
		auto pBag = pUserData->pBag;
		
		for (const auto& item : ans.user_data.bag)
		{
			std::shared_ptr<Item::Data> data = Item::Create(item.item_index, item.item_count);
			data->item_no = item.item_no;

			pBag->Insert(data);
		}

		auto pSuit = pUserData->pSuit;
		for (const auto& item : ans.user_data.suit)
		{
			std::shared_ptr<Item::Data> data = Item::Create(item.item_index, item.item_count);
			data->item_no = item.item_no;

			pSuit->Equip(data);
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Login",
	Message::User::MsgCliSvr_Login_Req, Test_Login_Req,
	Message::User::MsgSvrCli_Login_Ans, Test_Login_Ans
);





}}

