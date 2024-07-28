#include "Handler_Cheat.h"
#include "../Util/Cheat.h"

namespace Handler {

void Handler_Cheat::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::User::MsgCliSvr_Cheat_Req& req)
{
	Message::User::MsgSvrCli_Cheat_Ans ans;
	ans.error_code = Message::ErrorCode::Success;

	try {
		LOG(DEV, "Message::User::MsgCliSvr_Cheat_Req()");
		std::list<Cheat::Variant> params;
		for (const auto& param : req.params)
		{
			params.push_back(param);
		}
		ans.error_code = (Message::ErrorCode)Gamnet::Singleton<Cheat>::GetInstance().Command(req.command, session, params);
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (Message::ErrorCode)e.error_code();
	}
	LOG(DEV, "Message::User::MsgSvrCli_Cheat_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::User::MsgCliSvr_Cheat_Req,
	Handler_Cheat, Recv_Req,
	HandlerStatic
);

void Test_Cheat_Req(const std::shared_ptr<TestSession>& session)
{
}

void Test_Cheat_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_CreateItem",
	Message::User::MsgCliSvr_Cheat_Req, Test_Cheat_Req,
	Message::User::MsgSvrCli_Cheat_Ans, Test_Cheat_Ans
);

}