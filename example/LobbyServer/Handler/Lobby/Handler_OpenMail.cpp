#include "Handler_OpenMail.h"
#include "../../../idl/MessageLobby.h"
#include "../../Component/Mail.h"

namespace Handler { namespace Lobby {

Handler_OpenMail::Handler_OpenMail()
{
}

Handler_OpenMail::~Handler_OpenMail()
{
}

void Handler_OpenMail::Recv_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_OpenMail_Req req;
	MsgSvrCli_OpenMail_Ans ans;
	ans.error_code = ErrorCode::Success;
	
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "Lobby::MsgCliSvr_OpenMail_Req()");
		if(nullptr == session->GetComponent<UserData>())
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidUserError);
		}

		auto& mail = session->GetComponent<Component::Mail>();
		mail->Open(req.mail_seq);
		session->Commit();
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_User_OpenMailLobby_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_OpenMail_Req,
	Handler_OpenMail, Recv_Req,
	HandlerStatic
);

void Test_OpenMail_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_OpenMail_Req req;
	for(auto& itr : session->mails)
	{
		const ::MailData mail = itr.second;
		MsgCliSvr_OpenMail_Req req;
		req.mail_seq = mail.mail_seq;
		Gamnet::Test::SendMsg(session, req);
	}
}

void Test_OpenMail_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_OpenMail_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		//		LOG(INF, "[", session->link->link_manager->name, "/", session->link->link_key, "/", session->session_key, "] Test_UserUser_OpenMailLobby_Ans");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Lobby_OpenMail",
	MsgCliSvr_OpenMail_Req, Test_OpenMail_Req,
	MsgSvrCli_OpenMail_Ans, Test_OpenMail_Ans
);

}}