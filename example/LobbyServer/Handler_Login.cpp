#include "Handler_Login.h"
#include "Manager_Item.h"

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
	ans.user_data.user_id = "";
	ans.user_data.user_seq = 0;

	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_Login_Req(user_id:", req.user_id, ")");

		std::shared_ptr<UserData> userData = session->AddComponent<UserData>();

		Gamnet::Database::MySQL::ResultSet ret = Gamnet::Database::MySQL::Execute((int)Database::Gamnet,
			"SELECT user_seq FROM user_data WHERE user_id='", req.user_id, "'"
		);

		if(0 == ret.GetRowCount())
		{
			std::shared_ptr<ItemMeta> meta = Gamnet::Singleton<Manager_Item>::GetInstance().RandMeta();

			Gamnet::Database::MySQL::Transaction transaction((int)Database::Gamnet);
			transaction.Execute("INSERT INTO user_data(user_id) VALUES('", req.user_id, "')");
			transaction.Execute("INSERT INTO user_counter(counter_id, user_seq) VALUES(", 1, ", LAST_INSERT_ID())");
			transaction.Execute("INSERT INTO item_data(item_id, user_seq, item_count) VALUES(", meta->item_id, ", LAST_INSERT_ID(), 1)");
			transaction.Commit();

			ret = Gamnet::Database::MySQL::Execute((int)Database::Gamnet,
				"SELECT user_seq FROM user_data WHERE user_id='", req.user_id, "'"
			);
		}

		for(auto& row : ret)
		{
			userData->user_seq = row->getInt("user_seq");
			userData->user_id = req.user_id;
			session->transaction = std::make_shared<Gamnet::Database::MySQL::Transaction>((int)Database::Gamnet);
		}

		ret = Gamnet::Database::MySQL::Execute((int)Database::Gamnet,
			"SELECT counter_seq, counter_id, counter FROM user_counter WHERE user_seq='", userData->user_seq, "'"
		);

		for (auto& row : ret)
		{
			std::shared_ptr<Counter> counter = std::make_shared<Counter>(session, row->getUInt64("counter_seq"), row->getUInt32("counter_id"), row->getInt("counter"));
			session->AddCounter(counter);
		}

		session->GetCounter(1)->Increase(10);
		session->transaction->Commit();
		session->transaction->Clear();
		ans.user_data = *userData;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_Login_Ans(error_code:", (int)ans.error_code, ", user_seq:", ans.user_data.user_seq, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_Login_Req,
	Handler_Login, Recv_Req,
	HandlerStatic
);

void Test_Login_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_Login_Req req;
	req.user_id = session->session_token;
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
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Login",
	MsgCliSvr_Login_Req, Test_Login_Req,
	MsgSvrCli_Login_Ans, Test_Login_Ans
);

