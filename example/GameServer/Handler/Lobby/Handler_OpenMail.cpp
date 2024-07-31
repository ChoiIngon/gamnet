#include "Handler_OpenMail.h"
#include "../../Component/Account.h"
#include "../../Component/UserData.h"
#include "../../Component/Mail.h"

namespace Handler { namespace Lobby {

Handler_OpenMail::Handler_OpenMail()
{
}

Handler_OpenMail::~Handler_OpenMail()
{
}

void Handler_OpenMail::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Lobby::MsgCliSvr_OpenMail_Req& req)
{
	Message::Lobby::MsgSvrCli_OpenMail_Ans ans;
	ans.error_code = Message::ErrorCode::Success;
	
	try {
		LOG(DEV, "Message::Lobby::MsgCliSvr_OpenMail_Req()");
		/*
		if (nullptr == session->GetComponent<Component::Account>())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		ans.mail_seq = req.mail_seq;
		auto mail = session->GetComponent<Component::Mail>();

		session->StartTransaction();
		mail->Open(req.mail_seq);
		session->Commit();
		*/
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (Message::ErrorCode)e.error_code();
	}
	LOG(DEV, "Message::Lobby::MsgSvrCli_User_OpenMailLobby_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::Lobby::MsgCliSvr_OpenMail_Req,
	Handler_OpenMail, Recv_Req,
	HandlerStatic
);

void Test_OpenMail_Req(const std::shared_ptr<TestSession>& session)
{
	Message::Lobby::MsgCliSvr_OpenMail_Req req;
	req.mail_seq = 0;
	Gamnet::Test::SendMsg(session, req);
}

void Test_OpenMail_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Lobby::MsgSvrCli_OpenMail_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}

		if(0 == ans.mail_seq)
		{
			session->mails.clear();
		}
		else
		{
			session->mails.erase(ans.mail_seq);
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Lobby_OpenMail",
	Message::Lobby::MsgCliSvr_OpenMail_Req, Test_OpenMail_Req,
	Message::Lobby::MsgSvrCli_OpenMail_Ans, Test_OpenMail_Ans
);

}}