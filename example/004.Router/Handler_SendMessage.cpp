#include "Handler_SendMessage.h"

static std::atomic<uint32_t> MESSAGE_SEQ;

Handler_SendMessage::Handler_SendMessage() {
}

Handler_SendMessage::~Handler_SendMessage() {
}

void Handler_SendMessage::Recv_CliSvr_Req(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_SendMessage_Req reqCliSvr;
	MsgSvrSvr_SendMessage_Req reqSvrSvr;
	MsgSvrCli_SendMessage_Ans ansSvrCli;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(reqCliSvr, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(INF, "--- [RECV] MsgCliSvr_SendMessage_Req(session_key:", session->session_key, ", message:", reqCliSvr.text, ")");
		reqSvrSvr.text = reqCliSvr.text;

		std::string serviceName = "ROUTER_1";
		if ("ROUTER_1" == Gamnet::Network::Router::GetRouterAddress().service_name)
		{
			serviceName = "ROUTER_2";
		}
		else if("ROUTER_2" == Gamnet::Network::Router::GetRouterAddress().service_name)
		{
			serviceName = "ROUTER_1";
		}
		else
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidSeviceName, "(service_name:", Gamnet::Network::Router::GetRouterAddress().service_name, ")");
		}

		this->session = session;

		Gamnet::Network::Router::Address dest(Gamnet::Network::Router::ROUTER_CAST_TYPE::ANY_CAST, serviceName, 0);
		LOG(INF, "--- [SEND] MsgSvrSvr_SendMessage_Req(router_address:", dest.ToString(), ", message:", reqSvrSvr.text, ")");
		
		auto s = Gamnet::Singleton<Gamnet::Network::Router::RouterCaster>::GetInstance().FindSession(dest);
		if (nullptr == s)
		{
			return;
		}

		std::shared_ptr<Gamnet::Network::Tcp::Packet> buffer = Gamnet::Network::Tcp::Packet::Create();

		buffer->Write(reqSvrSvr);

		Gamnet::Network::Router::MsgRouter_SendMsg_Ntf ntf;
		ntf.msg_seq = ++s->send_seq;
		std::copy(buffer->ReadPtr(), buffer->ReadPtr() + buffer->Size(), std::back_inserter(ntf.buffer));

		std::shared_ptr<Gamnet::Network::Tcp::Packet> send = Gamnet::Network::Tcp::Packet::Create();
		send->Write(ntf);
		std::shared_ptr<Gamnet::Network::Tcp::Packet> result = s->SyncSend(send);
		if(nullptr == result)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidSeviceName, "(service_name:", Gamnet::Network::Router::GetRouterAddress().service_name, ")");
		}
		ntf.msg_seq = 0;
		ntf.buffer.clear();
		Gamnet::Network::Tcp::Packet::Load(ntf, result);

		MsgSvrSvr_SendMessage_Ans ansSvrSvr;
		std::shared_ptr<Gamnet::Network::Tcp::Packet> recvPacket = Gamnet::Network::Tcp::Packet::Create();
		recvPacket->Append(ntf.buffer.data(), ntf.buffer.size());
		Gamnet::Network::Tcp::Packet::Load(ansSvrSvr, recvPacket);
		LOG(INF, "--- [RECV] MsgSvrSvr_SendMessage_Ans(router_address:", s->router_address.ToString(), ", error_code:", (int)ansSvrSvr.error_code, ")");
		/*
		Gamnet::Network::Router::SyncSend(dest, reqSvrSvr, ansSvrSvr, 5);
		ansSvrCli.error_code = ansSvrSvr.error_code;
		*/
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ansSvrCli.error_code = (ErrorCode)e.error_code();
	}
	LOG(INF, "--- [SEND] MsgSvrCli_SendMessage_Ans(session_key:", session->session_key, ", error_code:", (int)ansSvrCli.error_code, ")");
	Gamnet::Network::Tcp::SendMsg(session, ansSvrCli);
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_SendMessage_Req,
	Handler_SendMessage, Recv_CliSvr_Req,
	HandlerCreate
);

void Handler_SendMessage::Recv_SvrSvr_Req(const std::shared_ptr<Gamnet::Network::Router::Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrSvr_SendMessage_Req reqSvrSvr;
	MsgSvrSvr_SendMessage_Ans ansSvrSvr;
	ansSvrSvr.error_code = ErrorCode::Success;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(reqSvrSvr, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(INF, "--- [RECV] MsgSvrSvr_SendMessage_Req(router_address:", session->router_address.ToString(), ", message:", reqSvrSvr.text, ")");
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ansSvrSvr.error_code = (ErrorCode)e.error_code();
	}
	LOG(INF, "--- [SEND] MsgSvrSvr_SendMessage_Ans(router_address:", session->router_address.ToString(), ", error_code:", (int)ansSvrSvr.error_code, ")");
	//Gamnet::Network::Router::SendMsg(session, ansSvrSvr);
}

GAMNET_BIND_ROUTER_HANDLER(
	MsgSvrSvr_SendMessage_Req,
	Handler_SendMessage, Recv_SvrSvr_Req,
	HandlerStatic
);

void Handler_SendMessage::Recv_SvrSvr_Ans(const std::shared_ptr<Gamnet::Network::Router::Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrSvr_SendMessage_Ans ansSvrSvr;
	MsgSvrCli_SendMessage_Ans ansSvrCli;
	ansSvrCli.error_code = ErrorCode::Success;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ansSvrSvr, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		ansSvrCli.error_code = ansSvrSvr.error_code;
		LOG(INF, "--- [RECV] MsgSvrSvr_SendMessage_Ans(router_address:", session->router_address.ToString(), ", error_code:", (int)ansSvrSvr.error_code, ")");
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
		ansSvrSvr.error_code = (ErrorCode)e.error_code();
	}
	Gamnet::Network::Tcp::SendMsg(this->session, ansSvrCli);
	LOG(INF, "--- [SEND] MsgSvrSvr_SendMessage_Ans(session_key:", this->session->session_key, ", error_code:", (int)ansSvrSvr.error_code, ")");
}

GAMNET_BIND_ROUTER_HANDLER(
	MsgSvrSvr_SendMessage_Ans,
	Handler_SendMessage, Recv_SvrSvr_Ans,
	HandlerFind
);

void Handler_SendMessage::Recv_CliSvr_Ntf(const std::shared_ptr<UserSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgCliSvr_SendMessage_Ntf ntfCliSvr;
	MsgSvrSvr_SendMessage_Ntf ntfSvrSvr;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntfCliSvr, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}

		LOG(INF, " --- MsgCliSvr_SendMessage_Ntf(session_key:", session->session_key, ", message:", ntfCliSvr.text, ")");
		ntfSvrSvr.text = ntfCliSvr.text;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	{
		Gamnet::Network::Router::Address dest(Gamnet::Network::Router::ROUTER_CAST_TYPE::ANY_CAST, "ROUTER_1", 0);
		Gamnet::Network::Router::SendMsg(dest, ntfSvrSvr);
	}
	{
		Gamnet::Network::Router::Address dest(Gamnet::Network::Router::ROUTER_CAST_TYPE::MULTI_CAST, "ROUTER_2", 0);
		Gamnet::Network::Router::SendMsg(dest, ntfSvrSvr);
	}
}

GAMNET_BIND_TCP_HANDLER(
	UserSession,
	MsgCliSvr_SendMessage_Ntf,
	Handler_SendMessage, Recv_CliSvr_Ntf,
	HandlerCreate
);

void Handler_SendMessage::Recv_SvrSvr_Ntf(const std::shared_ptr<Gamnet::Network::Router::Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrSvr_SendMessage_Ntf ntfSvrSvr;
	MsgSvrCli_SendMessage_Ntf ntfSvrCli;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntfSvrSvr, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		LOG(INF, "RECV MsgSvrSvr_SendMessage_Ntf(from:", session->router_address.service_name, ", to:", Gamnet::Network::Router::GetRouterAddress().service_name, ", message:", ntfSvrSvr.text, ")");
		ntfSvrCli.text = ntfSvrSvr.text;
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	LOG(INF, "SEND MsgSvrCli_SendMessage_Ntf(from:", Gamnet::Network::Router::GetRouterAddress().service_name, ", message:", ntfSvrCli.text, ")");
	Gamnet::Singleton<Manager_Session>::GetInstance().SendMsg(ntfSvrCli);
}

GAMNET_BIND_ROUTER_HANDLER(
	MsgSvrSvr_SendMessage_Ntf, 
	Handler_SendMessage, Recv_SvrSvr_Ntf, 
	HandlerStatic
);

void Test_CliSvr_SendMessage_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_SendMessage_Req req;
	req.text = "Hello World";
	//LOG(INF, "[C->S/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_SendMessage_Ntf(message:", ntf.Message, ")");
	session->pause_timer = Gamnet::Time::Timer::Create();
	session->pause_timer->SetTimer(5000, [session, req]() {
		Gamnet::Test::SendMsg(session, req);
	});
}

void Test_SvrCli_SendMessage_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_SendMessage_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		//LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_SendMessage_Ntf(message:", ntf.Message, ")");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_SendMessage_WithResponse",
	MsgCliSvr_SendMessage_Req, Test_CliSvr_SendMessage_Req,
	MsgSvrCli_SendMessage_Ans, Test_SvrCli_SendMessage_Ans
);

void Test_CliSvr_SendMessage_Ntf(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_SendMessage_Ntf ntf;
	ntf.text = "Hello World";
	//LOG(INF, "[C->S/", session->link->link_key, "/", session->session_key, "] MsgCliSvr_SendMessage_Ntf(message:", ntf.Message, ")");
	Gamnet::Test::SendMsg(session, ntf);
}

void Test_SvrCli_SendMessage_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_SendMessage_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail");
		}
		//LOG(INF, "[S->C/", session->link->link_key, "/", session->session_key, "] MsgSvrCli_SendMessage_Ntf(message:", ntf.Message, ")");
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_SendMessage",
	MsgCliSvr_SendMessage_Ntf, Test_CliSvr_SendMessage_Ntf, 
	MsgSvrCli_SendMessage_Ntf, Test_SvrCli_SendMessage_Ntf
);

/*
static std::mutex lock;
static std::set<Gamnet::Network::Router::Address> addresses;
static Gamnet::Timer timer;
void StartRouterMessageTimer()
{
	timer.AutoReset(false);
	timer.SetTimer(10000, []() {
		std::lock_guard<std::mutex> lo(lock);
		for (auto& itr : addresses)
		{
			MsgSvrSvr_SendMessage_Req req;
			req.Message = "Hello World";
			LOG(INF, "SEND MsgSvrSvr_SendMessage_Req(from:", Gamnet::Network::Router::GetRouterAddress().service_name, ", to:", itr.service_name, ", message:", req.Message, ")");
			Gamnet::Network::Router::SendMsg(itr, req);
		}
		StartRouterMessageTimer();
	});
}

*/