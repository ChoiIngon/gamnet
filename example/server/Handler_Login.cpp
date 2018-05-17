/*
 * Handler_Login.cpp
 *
 *  Created on: 2017. 8. 12.
 *      Author: kukuta
 */

#include "Handler_Login.h"
#include "Manager_Session.h"

Handler_Login::Handler_Login() {
}

Handler_Login::~Handler_Login() {
}

void Handler_Login::Recv_Req(const std::shared_ptr<Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_Login_Req req;
	MsgSvrCli_Login_Ans ans;
	ans.error_code = ErrorCode::Success;
	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(req, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_Login_Req(session_key:", session->session_key, ", user_id:", req.user_id, ")");

		if(0 != session->user_data.user_seq)
		{
			throw GAMNET_EXCEPTION(ErrorCode::AlreadyLoginSessionError, "session_key:", session->session_key);
		}

		const std::shared_ptr<Session> otherSession = Gamnet::Singleton<Manager_Session>::GetInstance().Add(req.user_id, session);
		if(nullptr != otherSession)
		{
			LOG(WRN, "duplicated session(user_id:", req.user_id, ")");
			MsgSvrCli_Kickout_Ntf ntf;
			ntf.error_code = ErrorCode::DuplicateConnectionError;
			LOG(DEV, "MsgSvrCli_Kickout_Ntf(session_key:", otherSession->session_key, ")");
			Gamnet::Network::Tcp::SendMsg(otherSession, ntf);
			otherSession->user_data.user_seq = 0;
		}
		
		UserData& user_data = session->user_data;
		user_data.user_id = req.user_id;
		user_data.user_seq = Gamnet::Random::Range(1, 99999);
		int count = Gamnet::Random::Range(1, 10);
		user_data.items.clear();
		for(int i=0;i<count; i++)
		{
			ItemData item;
			item.item_id = Gamnet::Format("item_id_", i);
			item.item_seq = Gamnet::Random::Range(1, 99999);
			user_data.items.push_back(item);
		}
		ans.user_data = user_data;
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = (ErrorCode)e.error_code();
	}
	LOG(DEV, "MsgSvrCli_Login_Ans(session_key:", session->session_key, ", user_seq:", ans.user_data.user_seq, ", error_code:", (int)ans.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ans);
}

GAMNET_BIND_TCP_HANDLER(
	Session,
	MsgCliSvr_Login_Req,
	Handler_Login, Recv_Req,
	HandlerCreate
);

void Test_Login_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_Login_Req req;
	req.user_id = Gamnet::Format("user_id_", Gamnet::Random::Range(1, 99999));
	Gamnet::Test::SendMsg(session, req);
}

static std::atomic_uint msg_seq;
void Test_Login_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Login_Ans ans;
	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		if(10 < ans.user_data.items.size())
		{
			LOG(ERR, "item data size over(size:", ans.user_data.items.size(), ")");
			assert(!"item size over");
		}
		session->user_data = ans.user_data;

	}
	catch(const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	//session->Reconnect();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Login",
	MsgCliSvr_Login_Req, MsgSvrCli_Login_Ans,
	Test_Login_Req, Test_Login_Ans
);
