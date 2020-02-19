#include "HttpServer.h"

namespace Gamnet { namespace Network { namespace Http {

	static Acceptor<Link> acceptor;

	void Listen(int port, int accept_queue_size)
	{
		acceptor.Listen(port, 1024, accept_queue_size);
		LOG(GAMNET_INF, "Gamnet::Http listener start(port:", port, ")");
	}

	void SendMsg(const std::shared_ptr<Session>& session, const Response& res)
	{
		if(nullptr == session)
		{
			LOG(ERR, "invalid session");
			return;
		}
		session->Send(res);
	}
}}}


