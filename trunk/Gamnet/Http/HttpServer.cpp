/*
 * HttpServer.cpp
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#include "HttpServer.h"

namespace Gamnet { namespace Http {
	void Listen(int port, int max_session, int keep_alive)
	{
		Singleton<Listener>::GetInstance().Init(port, max_session, keep_alive);
		Log::Write(GAMNET_INF, "Gamnet Http listener start(port:", port, ", capacity:", max_session, ", keep alive time:", keep_alive, "sec)");
	}

	bool SendMsg(std::shared_ptr<Session> session, const Response& res)
	{
		if(0 > session->Send(res))
		{
			Log::Write(GAMNET_ERR, "fail to send message(session_key:", session->sessionKey_, ")");
			return false;
		}
		return true;
	}
}}


