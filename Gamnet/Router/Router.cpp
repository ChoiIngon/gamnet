/*
 * Router.cpp
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#include "Router.h"
#include "../Library/Singleton.h"
#include "../Log/Log.h"

namespace Gamnet { namespace Router {

void Listen(const char* service_name, int port, int max_session)
{
	Singleton<RouterListener>().Init(service_name, port);
	Log::Write(GAMNET_INF, "Gamnet router start(service_name:", service_name, ", port:", port, ", capacity:", max_session, ", id:", Singleton<RouterListener>().localAddr_.id, ")");
}

void Connect(const char* host, int port, int timeout)
{
	Singleton<RouterListener>().Connect(host, port, timeout);
}

}}
