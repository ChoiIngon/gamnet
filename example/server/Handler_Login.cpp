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

void Handler_Login::Recv_Req(std::shared_ptr<Session> session, std::shared_ptr<Gamnet::Network::Packet> packet)
{
	MsgCliSvr_Login_Req req;
	MsgSvrCli_Login_Ans ans;
	ans.error_code = 0;
	try {
		if(false == Gamnet::Network::Packet::Load(req, packet))
		{
			LOG(ERR, "message load fail");
			throw Gamnet::Exception(-1, "message load fail");
		}

		LOG(DEV, "MsgCliSvr_Login_Req(user_id:", req.user_id, ")");
		if("" == req.access_token)
		{
			session->user_data = std::shared_ptr<UserData>(new UserData());
			const std::shared_ptr<UserData> user_data = session->user_data;
			user_data->user_id = req.user_id;
			user_data->user_seq = Gamnet::Random::Range(1, 99999);
			user_data->access_token = Gamnet::md5(Gamnet::Format(user_data->user_seq, time(NULL), Gamnet::Network::GetLocalAddress().to_string()));

			int count = Gamnet::Random::Range(1, 10);
			for(int i=0;i<count; i++)
			{
				ItemData item;
				item.item_id = Gamnet::Format("item_id_", i);
				item.item_seq = Gamnet::Random::Range(1, 99999);
				user_data->items.push_back(item);
			}
		}
		else
		{
			session->user_data = Gamnet::Singleton<Manager_Session>::GetInstance().Find(req.access_token);
			if(NULL == session->user_data)
			{
				throw Gamnet::Exception(-1, "invalid reconnect");
			}
			LOG(DEV, "success reconnect(user_id:", session->user_data->user_id, ")");
			Gamnet::Singleton<Manager_Session>::GetInstance().Remove(session->user_data->access_token);
		}
		ans.user_data = *(session->user_data);
	}
	catch(const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ans.error_code = e.error_code();
	}
	LOG(DEV, "MsgSvrCli_Login_Ans(user_seq:", ans.user_data.user_seq, ", error_code:", ans.error_code, ")");
	Gamnet::Network::SendMsg(session, ans);
}

GAMNET_BIND_NETWORK_HANDLER(
	Session,
	MsgCliSvr_Login_Req,
	Handler_Login, Recv_Req,
	HandlerCreate
);

void Test_Login_Req(std::shared_ptr<TestSession> session)
{
	MsgCliSvr_Login_Req req;
	req.user_id = Gamnet::Format("user_id_", Gamnet::Random::Range(1, 99999));
	Gamnet::Network::SendMsg(session, req);
}

void Test_Login_Ans(std::shared_ptr<TestSession> session, std::shared_ptr<Gamnet::Network::Packet> packet)
{
	MsgSvrCli_Login_Ans ans;
	try {
		if(false == Gamnet::Network::Packet::Load(ans, packet))
		{
			LOG(ERR, "message load fail");
			throw Gamnet::Exception(-1, "message load fail");
		}
		session->user_data = ans.user_data;
		LOG(INF, "error_code:", ans.error_code, ", user_seq:", session->user_data.user_seq);
		for(auto itr : session->user_data.items)
		{
			LOG(INF, "item_id:", itr.item_id, ", item_seq:", itr.item_seq);
		}
	}
	catch(const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_HANDLER(
	TestSession,
	MsgCliSvr_Login_Req, MsgSvrCli_Login_Ans,
	Test_Login_Req, Test_Login_Ans
);
