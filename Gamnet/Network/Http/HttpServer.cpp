#include "HttpServer.h"

namespace Gamnet { namespace Network { namespace Http {

	void Listen(int port)
	{
		Singleton<LinkManager>::GetInstance().Listen(port);
		LOG(GAMNET_INF, "Gamnet::Http listener start(port:", port, ")");
	}

	void SendMsg(const std::shared_ptr<Session>& session, const Response& res)
	{
		session->Send(res);
	}
}}}


