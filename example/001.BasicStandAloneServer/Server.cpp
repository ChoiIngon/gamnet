#include <Gamnet/Gamnet.h>
#include "Message.h"

class Session : public Gamnet::Network::Tcp::Session
{
public:
	Session() = default;
	virtual ~Session() = default;

	virtual void OnCreate() override
	{
		LOG(DEV, "session_key:", session_key);

		MsgSvrCli_Welcome_Ntf ntf;
		ntf.greeting = "Welcome";
		Gamnet::Network::Tcp::SendMsg(std::static_pointer_cast<Session>(shared_from_this()), ntf);
	}
	virtual void OnAccept() override
	{
		LOG(DEV, "session_key:", session_key);
	}
	virtual void OnClose(int reason) override
	{
		LOG(DEV, "session_key:", session_key, ", reason:", reason);
	}
	virtual void OnDestroy() override
	{
		LOG(DEV, "session_key:", session_key);
	}
};

class Handler_HelloWorld : public Gamnet::Network::IHandler {
public:
	Handler_HelloWorld() = default;
	virtual ~Handler_HelloWorld() = default;

	void Recv_CliSvr_Req(const std::shared_ptr<Session>& session, const MsgCliSvr_HelloWorld_Req& req)
	{
		MsgSvrCli_HelloWorld_Ans ans;
		try {
			LOG(DEV, "MsgCliSvr_HelloWorld_Req(session_key:", session->session_key, ", message:", req.greeting, ")");
			ans.answer = req.greeting;
		}
		catch (const Gamnet::Exception& e)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
			ans.answer = e.what();
		}
		LOG(DEV, "MsgSvrCli_HelloWorld_Ans(session_key:", session->session_key, ", message:", ans.answer, ")");
		Gamnet::Network::Tcp::SendMsg(session, ans);
	}
};

GAMNET_BIND_TCP_HANDLER(
	Session,
	MsgCliSvr_HelloWorld_Req,
	Handler_HelloWorld, Recv_CliSvr_Req,
	HandlerStatic
);

class TestSession : public Gamnet::Test::Session {
public:
	virtual void OnCreate() override
	{
	}
	virtual void OnConnect() override
	{
	}
	virtual void OnClose(int reason) override
	{
	}
	virtual void OnDestroy() override
	{
	}
};

void Test_CliSvr_HelloWorld_Req(const std::shared_ptr<TestSession>& session)
{
	MsgCliSvr_HelloWorld_Req req;
	req.greeting = "World Hello";
	Gamnet::Test::SendMsg(session, req);
}

void Test_SvrCli_HelloWorld_Ans(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_HelloWorld_Ans ans;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ans, packet))
		{
			throw GAMNET_EXCEPTION(-1, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
	session->Next();
}

GAMNET_BIND_TEST_HANDLER(
	TestSession, "Test_HelloWorld",
	MsgCliSvr_HelloWorld_Req, Test_CliSvr_HelloWorld_Req,
	MsgSvrCli_HelloWorld_Ans, Test_SvrCli_HelloWorld_Ans
);

void Test_SvrCli_Welcome_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	MsgSvrCli_Welcome_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(-1, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	MsgSvrCli_Welcome_Ntf, Test_SvrCli_Welcome_Ntf
);

int main(int argc, char** argv)
{
	const std::string config = "config.xml";

	Gamnet::InitCrashDump();
	try {
		Gamnet::Log::ReadXml(config);
		LOG(INF, "file:", argv[0]);
		LOG(INF, "build date:", __DATE__, " ", __TIME__);
		LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

		Gamnet::Network::Tcp::ReadXml<Session>(config);
		Gamnet::Test::ReadXml<TestSession>(config);
		Gamnet::Run(std::thread::hardware_concurrency());
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		return -1;
	}
	
	return 0;
}
