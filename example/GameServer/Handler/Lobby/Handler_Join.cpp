#include "Handler_Join.h"
#include "../../Component/UserData.h"
#include "../../Component/Event.h"
#include "../../Component/Mail.h"

namespace Handler { namespace Lobby {

Handler_Join::Handler_Join()
{
}

Handler_Join::~Handler_Join()
{
}


void Handler_Join::Recv_Req(const std::shared_ptr<UserSession>& session, const Message::Lobby::MsgCliSvr_Join_Req& req)
{
	Message::Lobby::MsgSvrCli_Join_Ans ans;
	ans.error_code = Message::ErrorCode::Success;
	
	
	try {
		LOG(DEV, "Message::Lobby::MsgCliSvr_Join_Req()");
		if (nullptr == session->GetComponent<Component::UserData>())
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidUserError);
		}

		LoadUserEvent(session);
		LoadUserMail(session);
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (Message::ErrorCode)e.error_code();
	}
	LOG(DEV, "Message::Lobby::MsgSvrCli_Join_Ans(error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	Message::Lobby::MsgCliSvr_Join_Req,
	Handler_Join, Recv_Req,
	HandlerStatic
);

void Handler_Join::LoadUserEvent(const std::shared_ptr<UserSession>& session)
{
	std::shared_ptr<Component::Event> evt = session->GetComponent<Component::Event>();
	session->StartTransaction();
	evt->Load();
	session->Commit();
}

void Handler_Join::LoadUserMail(const std::shared_ptr<UserSession>& session)
{
	std::shared_ptr<Component::Mail> mail = session->GetComponent<Component::Mail>();
	mail->Load();
}

void Test_Join_Req(const std::shared_ptr<TestSession>& session)
{
	Message::Lobby::MsgCliSvr_Join_Req req;
	Gamnet::Test::SendMsg(session, req);
}

void Test_Join_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Lobby::MsgSvrCli_Join_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
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
	Message::Lobby::MsgCliSvr_Join_Req, Test_Join_Req,
	Message::Lobby::MsgSvrCli_Join_Ans, Test_Join_Ans
);

void Test_Mail_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Lobby::MsgSvrCli_Mail_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
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
	Message::Lobby::MsgSvrCli_Mail_Ntf, Test_Mail_Ntf
);

}}