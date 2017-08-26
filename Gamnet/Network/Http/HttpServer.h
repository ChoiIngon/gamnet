/*
 * HttpServer.h
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HTTP_HTTPSERVER_H_
#define GAMNET_NETWORK_HTTP_HTTPSERVER_H_

#include "Dispatcher.h"
#include "HttpClient.h"
#include "LinkManager.h"
#include "Request.h"

namespace Gamnet { namespace Network { namespace Http {
	void Listen(int port, int max_session, int keep_alive);
	template <class FUNC, class FACTORY>
	bool RegisterHandler(const char* uri, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher>::GetInstance().RegisterHandler(uri, func, factory);
	}

	bool SendMsg(const std::shared_ptr<Session>& session, const Response& res);
}}}

#define GAMNET_BIND_HTTP_HANDLER(uri, class_type, func) \
	static bool Http_##class_type##_##func = Gamnet::Network::Http::RegisterHandler( \
			uri, \
			&class_type::func, \
			new Gamnet::Network::HandlerStatic<class_type>() \
	)

#endif /* HTTPSERVER_H_ */
