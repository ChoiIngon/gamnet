#include <boost/program_options.hpp>
#include <Network/Tcp/Session.h>
#include <Network/Tcp/SessionManager.h>

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

int main(int argc, char** argv) 
{
	Gamnet::Network::Tcp::SessionManager<Session> session_manager;
	session_manager.Listen(9999, 1000, 300, 500, 5);
	
	session_manager.io_service.run();
	

	return 0;
}


