#include "Handler_Join.h"
#include "../../../idl/MessageLobby.h"
#include "../../Component/Event.h"
#include "../../Component/Mail.h"

namespace Handler { namespace Lobby {

Handler_Join::Handler_Join()
{
}

Handler_Join::~Handler_Join()
{
}

void Handler_Join::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_Join_Req req;
	MsgSvrCli_Join_Ans ans;
	ans.error_code = ErrorCode::Success;
	
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "Lobby::MsgCliSvr_Join_Req()");
		if(nullptr == session->GetComponent<UserData>())
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError);
		}

		LoadUserEvent(session);
		LoadUserMail(session);
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
	MsgCliSvr_Join_Req,
	Handler_Join, Recv_Req,
	HandlerStatic
);

void Handler_Join::LoadUserEvent(const std::shared_ptr<UserSession>& session)
{
	std::shared_ptr<Component::Event> evt = session->GetComponent<Component::Event>();
	evt->Load();
}

void Handler_Join::LoadUserMail(const std::shared_ptr<UserSession>& session)
{
	std::shared_ptr<Component::Mail> mail = session->GetComponent<Component::Mail>();
	mail->Load();
}

void Test_Join_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_Join_Req req;
	Gamnet::Test::SendMsg(session, req);
}

void Test_Join_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Join_Ans ans;
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
	TestSession, "Test_Lobby_Join",
	MsgCliSvr_Join_Req, Test_Join_Req,
	MsgSvrCli_Join_Ans, Test_Join_Ans
);

void Test_Mail_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Mail_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_JoinLobby_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	for (auto& mail : ntf.mail_datas)
	{
		session->mails.insert(std::make_pair(mail.mail_seq, mail));
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	MsgSvrCli_Mail_Ntf, Test_Mail_Ntf
);

}}