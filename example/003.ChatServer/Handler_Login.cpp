/*
 * Handler_Login.cpp
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#include "Handler_Login.h"

Handler_Login::Handler_Login() {
}

Handler_Login::~Handler_Login() {
}

void Handler_Login::Recv_Req(const std::shared_ptr<ChatSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_Login_Req req;
	MsgSvrCli_Login_Ans ans;
	ans.Error = ErrorCode::Success;

	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		const std::shared_ptr<ChatSession> otherSession = Gamnet::Singleton<Manager_Session>::GetInstance().Add(req.UserID, session);
		if(nullptr != otherSession)
		{
			MsgSvrCli_Kickout_Ntf ntf;
			ntf.Error = ErrorCode::DuplicateConnectionError;
			Gamnet::Network::Tcp::SendMsg(otherSession, ntf, true);
		}
		
		ChatUserData& userData = session->user_data;
		userData.UserID = req.UserID;
		ans.UserData = userData;
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.Error = (ErrorCode)e.error_code();
	}
	
	Gamnet::Network::Tcp::SendMsg(session, ans, true);
}

GAMNET_BIND_TCP_HANDLER(
	ChatSession,
	MsgCliSvr_Login_Req,
	Handler_Login, Recv_Req,
	HandlerCreate
);

void Test_Login_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_Login_Req req;
	req.UserID = session->session_token;
	LOG(INF, "[C->S/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_Login_Req(user_id:", req.UserID, ")");
	Gamnet::Test::SendMsg(session, req);
}

void Test_Login_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Login_Ans ans;
	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		session->user_data = ans.UserData;
		LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_Login_Ans(error_code:", ToString<ErrorCode>(ans.Error), ")");
	}
	catch(const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Login",
	MsgCliSvr_Login_Req, MsgSvrCli_Login_Ans,
	Test_Login_Req, Test_Login_Ans
);

static std::string USER_IDS[] = {
	"UID001", "UID002", "UID003", "UID004", "UID005", "UID006", "UID007", "UID008", "UID009", "UID010",
	"UID011", "UID012", "UID013", "UID014", "UID015", "UID016", "UID017", "UID018", "UID019", "UID020",
	"UID021", "UID022", "UID023", "UID024", "UID025", "UID026", "UID027", "UID028", "UID029", "UID030",
	"UID031", "UID032", "UID033", "UID034", "UID035", "UID036", "UID037", "UID038", "UID039", "UID040",
	"UID041", "UID042", "UID043", "UID044", "UID045", "UID046", "UID047", "UID048", "UID049", "UID050"
};

void Test_DuplicateLogin_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_Login_Req req;
	req.UserID = USER_IDS[Gamnet::Random::Range(0, USER_IDS->size())];
	Gamnet::Test::SendMsg(session, req);
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_DuplicateLogin",
	MsgCliSvr_Login_Req, MsgSvrCli_Login_Ans,
	Test_DuplicateLogin_Req, Test_Login_Ans
);

void Test_Kickout_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Kickout_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->link->strand.wrap(std::bind(&Gamnet::Network::Link::Close, session->link, (int)ntf.Error));
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, 
	MsgSvrCli_Kickout_Ntf,
	Test_Kickout_Ntf
);
