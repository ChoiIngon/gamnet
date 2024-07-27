#include "Handler_Create.h"

#include "../../Component/Account.h"
#include "../../Util/MetaData.h"

namespace Handler { namespace User {

class ShardIndex
{
public:
	struct ShardData : public MetaData
	{
		ShardData();

		int shard_num;
		int shard_index;
		std::string master_ip;
		int master_port;
		std::string db_name;
		std::string db_user;
		std::string db_password;
	};

	void Init();
	int Generate();
private:
	std::atomic<int> shard_mod;
	MetaReader<ShardData> meta_reader;
};


void Handler_Create::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::User::MsgCliSvr_Create_Req& req)
{
	Message::User::MsgSvrCli_Create_Ans ans;
	ans.error_code = Message::ErrorCode::Success;

	try {
		LOG(DEV, "Message::User::MsgCliSvr_User_Create_Req(account_id:", req.account_id, ")");

		if(nullptr != session->GetComponent<Component::Account>())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}

		int shard_index = Gamnet::Singleton<ShardIndex>::GetInstance().Generate();
		Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
			"INSERT INTO Account(account_id, account_type, shard_index) "
			"VALUES('", req.account_id, "',", (int)req.account_type, ",", shard_index, ")"
		);

		auto rows = Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
			"SELECT user_no, shard_index FROM Account WHERE account_id = '", req.account_id, "' AND account_type = ", (int)req.account_type
		);

		if (1 != rows.GetRowCount())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError, "account_id:", req.account_id, ", account_type:", (int)req.account_type);
		}
		
		auto row = rows[0];
		int64_t user_no = row->getInt64("user_no");
		Gamnet::Database::MySQL::Execute(shard_index,
			"INSERT INTO UserData(user_no, user_name) "
			"VALUES(", user_no, ",'", req.user_name, "')"
		);
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		switch (e.error_code())
		{
		case 1061: // duplicated key
			ans.error_code = Message::ErrorCode::DuplicateNameError;
			break;
		default:
			ans.error_code = (Message::ErrorCode)e.error_code();
			break;
		}
	}
	LOG(DEV, "Message::User::MsgSvrCli_User_Create_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::User::MsgCliSvr_Create_Req,
	Handler_Create, Recv_Req,
	HandlerStatic
);

ShardIndex::ShardData::ShardData()
{
	META_MEMBER(shard_num);
	META_MEMBER(shard_index);
	META_MEMBER(master_ip);
	META_MEMBER(master_port);
	META_MEMBER(db_name);
	META_MEMBER(db_user);
	META_MEMBER(db_password);
}

void ShardIndex::Init()
{
	const auto& metas = meta_reader.Read("../MetaData/Shard.csv");
	int shard_count = (int)metas.size();
	if (0 == shard_count)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "no user database");
	}

	for (auto& meta : metas)
	{
		if (false == Gamnet::Database::MySQL::Connect((int)DatabaseType::User + meta->shard_index, meta->master_ip, meta->master_port, meta->db_user, meta->db_password, meta->db_name, false))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "fail to connect db");
		}
	}
	shard_mod = Gamnet::Random::Range(0, shard_count - 1);
}

int ShardIndex::Generate()
{
	const auto& metas = meta_reader.GetAllMetaData();
	size_t shard_count = metas.size();
	if (0 == shard_count)
	{
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "no user database");
	}

	return (int)DatabaseType::User + metas[shard_mod++ % shard_count]->shard_index;
}

GAMNET_BIND_INIT_HANDLER(ShardIndex, Init);



void Test_Create_Req(const std::shared_ptr<TestSession>& session)
{
	Message::User::MsgCliSvr_Create_Req req;
	req.user_name = session->session_token.substr(0, 16);
	req.account_id = session->session_token;
	req.account_type = Message::AccountType::Dev;
	Gamnet::Test::SendMsg(session, req);
}

void Test_Create_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::User::MsgSvrCli_Create_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_Create_Ans");

		if(Message::ErrorCode::Success != ans.error_code)
		{
			session->Send_Close_Req();
			return;
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "MsgCliSvr_Create_Req",
	Message::User::MsgCliSvr_Create_Req, Test_Create_Req,
	Message::User::MsgSvrCli_Create_Ans, Test_Create_Ans
);

/*
GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::User::MsgSvrCli_Create_Ans, Test_Create_Ans
);
*/
}}