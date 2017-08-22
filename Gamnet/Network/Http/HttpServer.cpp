#include "HttpServer.h"

namespace Gamnet { namespace Network { namespace Http {

	void Listen(int port, int max_session, int keep_alive)
	{
		Singleton<LinkManager>::GetInstance().Listen(port, max_session, keep_alive);
		LOG(GAMNET_INF, "Gamnet Http listener start(port:", port, ", capacity:", max_session, ", keep alive time:", keep_alive, " sec)");
	}

	bool SendMsg(std::shared_ptr<Session> session, const Response& res)
	{
		if(0 > session->Send(res))
		{
			LOG(GAMNET_ERR, "fail to send message(session_key:", session->session_key, ")");
			return false;
		}
		return true;
	}
}}}


