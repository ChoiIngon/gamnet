#include "HttpServer.h"

namespace Gamnet { namespace Network { namespace Http {

	void Listen(int port, int accept_queue_size)
	{
		Singleton<LinkManager>::GetInstance().Listen(port, accept_queue_size, 0);
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


