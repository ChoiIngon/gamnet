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
			throw Gamnet::Exception(GAMNET_ERRNO(ErrorCode::MessageFormatError), "message load fail");
		}

		LOG(DEV, "MsgCliSvr_Login_Req(session_key:", session->session_key, ", user_id:", req.user_id, ")");

		if(0 != session->user_data.user_seq)
		{
			throw Gamnet::Exception(GAMNET_ERRNO(ErrorCode::AlreadyLoginSessionError), "session_key:", session->session_key);
		}

		const std::shared_ptr<Session> other = Gamnet::Singleton<Manager_Session>::GetInstance().Add(req.user_id, session);
		if(NULL != other)
		{
			LOG(WRN, "duplicated session(user_id:", req.user_id, ")");
			MsgSvrCli_Kickout_Ntf ntf;
			ntf.error_code = ErrorCode::DuplicateConnectionError;
			LOG(DEV, "MsgSvrCli_Kickout_Ntf(session_key:", other->session_key, ")");
			Gamnet::Network::Tcp::SendMsg(other, ntf);
			const std::shared_ptr<Gamnet::Network::Link> link = other->link;
			if(NULL != link)
			{
				link->AttachSession(NULL);
			}
		}
		
		UserData& user_data = session->user_data;
		user_data.user_id = req.user_id;
		user_data.user_seq = Gamnet::Random::Range(1, 99999);
		int count = Gamnet::Random::Range(1, 10);
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
	LOG(DEV, "MsgSvrCli_Login_Ans(user_seq:", ans.user_data.user_seq, ", error_code:", (int)ans.error_code, ")");
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

void Test_Login_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Login_Ans ans;
	try {
		if(false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw Gamnet::Exception(GAMNET_ERRNO(ErrorCode::MessageFormatError), "message load fail");
		}
		session->user_data = ans.user_data;
		LOG(DEV, "user_seq:", session->user_data.user_seq, ", session_token:", session->session_token);
	}
	catch(const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_Login",
	MsgCliSvr_Login_Req, MsgSvrCli_Login_Ans,
	Test_Login_Req, Test_Login_Ans
);
