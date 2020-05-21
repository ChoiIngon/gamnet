#include <boost/program_options.hpp>
#include <Network/HandlerFactory.h>
#include <Network/Tcp/Session.h>
#include <Network/Tcp/SystemMessageHandler.h>
#include <Network/Tcp/Tcp.h>
#include <Test/SessionManager.h>


class Session : public Gamnet::Network::Tcp::Session {
public :
	virtual void OnCreate() override 
	{
	}
	virtual void OnAccept() override
	{
	}
	virtual void OnClose(int reason) override
	{
	}
	virtual void OnDestroy() override
	{
	}
};

class TestSession : public Gamnet::Test::Session {
public:
	virtual void OnCreate() override
	{
	}
	virtual void OnAccept() override
	{
	}
	virtual void OnClose(int reason) override
	{
	}
	virtual void OnDestroy() override
	{
	}
};

int main(int argc, char** argv) 
{
	//Gamnet::Test::Config config;
	//config.ReadXml("config.xml");
	//auto& condition = config.OnCondition("OnReceive", { {"msg_id", "Recv_Connect_Ans"} });

	Gamnet::Network::Tcp::Listen<Session>(9999, 1000, 300, 500, 2);
	Gamnet::Singleton<Gamnet::Test::SessionManager<TestSession>>::GetInstance().Init("127.0.0.1", 9999, 1, 1);
	Gamnet::Singleton<boost::asio::io_service>::GetInstance().run();
	return 0;
}


