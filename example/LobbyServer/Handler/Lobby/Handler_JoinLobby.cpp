#include "Handler_JoinLobby.h"
#include "../../../idl/MessageLobby.h"

namespace Handler { namespace Lobby {

Handler_JoinLobby::Handler_JoinLobby()
{
}

Handler_JoinLobby::~Handler_JoinLobby()
{
}

void Handler_JoinLobby::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_JoinLobby_Req req;
	MsgSvrCli_JoinLobby_Ans ans;
	ans.error_code = ErrorCode::Success;
	
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_User_JoinLobby_Req()");
		if(nullptr == session->GetComponent<UserData>())
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError);
		}


	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_User_JoinLobby_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_JoinLobby_Req,
	Handler_JoinLobby, Recv_Req,
	HandlerStatic
);

void Test_JoinLobby_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_JoinLobby_Req req;
	Gamnet::Test::SendMsg(session, req);
}

void Test_JoinLobby_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_JoinLobby_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_JoinLobby_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_JoinLobby",
	MsgCliSvr_JoinLobby_Req, Test_JoinLobby_Req,
	MsgSvrCli_JoinLobby_Ans, Test_JoinLobby_Ans
);

}}