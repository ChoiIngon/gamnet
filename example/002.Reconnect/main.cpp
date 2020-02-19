#include "UserSession.h"

int main() {
	Gamnet::Log::ReadXml("config.xml");
	LOG(INF, "002.Reconnect Example Server Starts..");
	LOG(INF, "build date:", __DATE__, " ", __TIME__);
	LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

	try {
		Gamnet::Network::Tcp::ReadXml<UserSession>("config.xml");
		Gamnet::Network::Router::ReadXml("config.xml");
		Gamnet::Network::Http::Listen(20002);

		Gamnet::Test::ReadXml<TestSession>("config.xml");
		Gamnet::Run(std::thread::hardware_concurrency());
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(ERR, e.what(), "(error_code:", e.error_code(), ")");
		return 1;
	}
	return 0;
}

class Http_ServerStateHandler : public Gamnet::Network::IHandler
{
public:
	Http_ServerStateHandler();
	virtual ~Http_ServerStateHandler();

	void Recv_Req(const std::shared_ptr<Gamnet::Network::Http::Session>& session, const Gamnet::Network::Http::Request& param);
};

Http_ServerStateHandler::Http_ServerStateHandler()
{
}


Http_ServerStateHandler::~Http_ServerStateHandler()
{
}

void Http_ServerStateHandler::Recv_Req(const std::shared_ptr<Gamnet::Network::Http::Session>& session, const Gamnet::Network::Http::Request& param)
{
	Gamnet::Network::Http::Response res;
	res.error_code = 200;

	try
	{
		Json::Value root = Gamnet::Network::Tcp::ServerState<UserSession>();
		Json::StyledWriter writer;
		res.context = writer.write(root);
	}
	catch (const Gamnet::Exception& e)
	{
		res.error_code = e.error_code();
		res.context = e.what();
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
	}
	Gamnet::Network::Http::SendMsg(session, res);
}

GAMNET_BIND_HTTP_HANDLER("server_state", Http_ServerStateHandler, Recv_Req);


