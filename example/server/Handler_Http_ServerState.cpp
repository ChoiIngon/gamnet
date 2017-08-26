#include "Handler_Http_ServerState.h"

Handler_Http_ServerState::Handler_Http_ServerState()
{
}


Handler_Http_ServerState::~Handler_Http_ServerState()
{
}

void Handler_Http_ServerState::Recv_Req(const std::shared_ptr<Gamnet::Network::Http::Session>& session, const Gamnet::Network::Http::Request& param)
{
	Gamnet::Network::Http::Response res;
	res.nErrorCode = 200;

	try
	{
		Json::Value root = Gamnet::Network::Tcp::ServerState<Session>("server");
		root["param"] = (const std::string)param["param"];

		Json::StyledWriter writer;
		res.sBodyContext = writer.write(root);
	}
	catch (const Gamnet::Exception& e)
	{
		res.nErrorCode = e.error_code();
		res.sBodyContext = e.what();
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
	}
	Gamnet::Network::Http::SendMsg(session, res);
}

GAMNET_BIND_HTTP_HANDLER("test/server_state", Handler_Http_ServerState, Recv_Req);


