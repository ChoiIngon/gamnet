#include "Handler_Create.h"
#include "../../../idl/MessageUser.h"

namespace Handler { namespace User {

	class ExistUserNameCache
	{
	public:
		ExistUserNameCache();
		void Init();
		static bool CheckExist(const std::string& name);
	private:
		const std::shared_ptr<std::set<std::string>> GetCache();
		std::shared_ptr<std::set<std::string>> Read();
		const int			page_size;
		uint64_t			last_user_seq;
		std::mutex			lock;
		Gamnet::Time::Timer	timer;
		std::shared_ptr<std::set<std::string>> cache;
	};

	class ShardIndex
	{
	public:
		struct ShardData : public Gamnet::MetaData
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
		static int Generate();
	private:
		std::atomic<int> shard_mod;
		Gamnet::MetaReader<ShardData> meta_reader;
		int _Generate();
	};


	Handler_Create::Handler_Create()
	{
	}

	Handler_Create::~Handler_Create()
	{
	}

	void Handler_Create::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
	{
		MsgCliSvr_Create_Req req;
		MsgSvrCli_Create_Ans ans;
		ans.error_code = ErrorCode::Success;

		try {
			if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
			}

			LOG(DEV, "MsgCliSvr_User_Create_Req(account_id:", req.account_id, ")");

			if (false == ExistUserNameCache::CheckExist(req.user_name))
			{
				throw GAMNET_EXCEPTION(ErrorCode::DuplicateNameError, "user_name:", req.user_name);
			}

			Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
				"INSERT INTO account(user_name, account_id, account_type, account_level, account_state, shard_index, create_date, delete_date) "
				"VALUES('", req.user_name, "','", req.account_id, "',", (int)req.account_type, ",1,1,", ShardIndex::Generate(), ",NOW(),'0000-00-00 00:00:00')"
			);
		}
		catch (const Gamnet::Exception& e)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
			switch (e.error_code())
			{
			case 1061: // duplicated key
				ans.error_code = ErrorCode::DuplicateNameError;
				break;
			default:
				ans.error_code = (ErrorCode)e.error_code();
				break;
			}
		}
		LOG(DEV, "MsgSvrCli_User_Create_Ans(error_code:", (int)ans.error_code, ")");
		Gamnet::Network::Tcp::SendMsg(session, ans);
	}

	GAMNET_BIND_TCP_HANDLER(
		UserSession,
		MsgCliSvr_Create_Req,
		Handler_Create, Recv_Req,
		HandlerStatic
	);

	ExistUserNameCache::ExistUserNameCache()
		: page_size(5000)
		, last_user_seq(0)
		, cache(std::make_shared<std::set<std::string>>())
	{
	}

	std::shared_ptr<std::set<std::string>> ExistUserNameCache::Read()
	{
		std::shared_ptr<std::set<std::string>> temp = std::make_shared<std::set<std::string>>();
		*temp = *cache;

		Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute((int)DatabaseType::Account,
			"SELECT user_seq, user_name FROM account WHERE user_seq > ", last_user_seq, " LIMIT ", page_size
		);
		for(auto& row : rows)
		{
			temp->insert(row->getString("user_name"));
			last_user_seq = std::max(last_user_seq, row->getUInt64("user_seq"));
		}
		return temp;
	}

	void ExistUserNameCache::Init()
	{
		std::shared_ptr<std::set<std::string>> temp = Read();
		{
			std::lock_guard<std::mutex> lo(lock);
			cache = temp;
		}

		timer.AutoReset(false);
		timer.SetTimer(60000, std::bind(&ExistUserNameCache::Init, this));
	}

	const std::shared_ptr<std::set<std::string>> ExistUserNameCache::GetCache()
	{
		std::lock_guard<std::mutex> lo(lock);
		return cache;
	}

	GAMNET_BIND_INIT_HANDLER(ExistUserNameCache, Init);

	bool ExistUserNameCache::CheckExist(const std::string& name)
	{
		auto cache = Gamnet::Singleton<ExistUserNameCache>::GetInstance().GetCache();
		auto itr = cache->find(name);
		if(cache->end() != itr)
		{
			return false;
		}
		return true;
	}

	ShardIndex::ShardData::ShardData()
	{
		GAMNET_META_MEMBER(shard_num);
		GAMNET_META_MEMBER(shard_index);
		GAMNET_META_MEMBER(master_ip);
		GAMNET_META_MEMBER(master_port);
		GAMNET_META_MEMBER(db_name);
		GAMNET_META_MEMBER(db_user);
		GAMNET_META_MEMBER(db_password);
	}

	void ShardIndex::Init()
	{
		const auto& metas = meta_reader.Read("../MetaData/Shard.csv");
		int shard_count = (int)metas.size();
		if (0 == shard_count)
		{
			throw GAMNET_EXCEPTION(ErrorCode::UndefineError, "no user database");
		}

		for (auto& meta : metas)
		{
			if (false == Gamnet::Database::MySQL::Connect((int)DatabaseType::User + meta->shard_index, meta->master_ip, meta->master_port, meta->db_user, meta->db_password, meta->db_name, false))
			{
				throw GAMNET_EXCEPTION(ErrorCode::UndefineError, "fail to connect db");
			}
		}
		shard_mod = Gamnet::Random::Range(0, shard_count - 1);
	}

	int ShardIndex::_Generate()
	{
		const auto& metas = meta_reader.GetAllMetaData();
		size_t shard_count = metas.size();
		if (0 == shard_count)
		{
			throw GAMNET_EXCEPTION(ErrorCode::UndefineError, "no user database");
		}

		return (int)DatabaseType::User + metas[shard_mod++ % shard_count]->shard_index;
	}

	int ShardIndex::Generate()
	{
		return Gamnet::Singleton<ShardIndex>::GetInstance()._Generate();
	}

	GAMNET_BIND_INIT_HANDLER(ShardIndex, Init);



void Test_Create_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_Create_Req req;
	req.user_name = session->session_token.substr(0, 16);
	req.account_id = session->session_token;
	req.account_type = AccountType::Dev;
	Gamnet::Test::SendMsg(session, req);
}

void Test_Create_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Create_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_Create_Ans");

		if(ErrorCode::Success != ans.error_code)
		{
			session->Send_Close_Req();
			return;
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	MsgCliSvr_Login_Req req;
	req.account_id = session->session_token;
	req.account_type = AccountType::Dev;
	Gamnet::Test::SendMsg(session, req);
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Create",
	MsgCliSvr_Create_Req, Test_Create_Req,
	MsgSvrCli_Create_Ans, Test_Create_Ans
);

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	MsgSvrCli_Create_Ans, Test_Create_Ans
);

}}