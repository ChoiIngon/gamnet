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
}}


