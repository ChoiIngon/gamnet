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

void Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose)
{
	Singleton<RouterListener>::GetInstance().Init(service_name, port, onAccept, onClose);
	Log::Write(GAMNET_INF, "Gamnet router start(service_name:", service_name, ", local ip:", Network::GetLocalAddress().to_string(), ", port:", port, ", id:", Singleton<RouterListener>::GetInstance().localAddr_.id, ")");
}

void Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose)
{
	Singleton<RouterListener>::GetInstance().Connect(host, port, timeout, onConnect, onClose);
}

}}
