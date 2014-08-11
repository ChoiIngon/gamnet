/*
 * HttpServer.h
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "Listener.h"
#include "Dispatcher.h"

namespace Gamnet { namespace Http {
	void Listen(int port, int max_session, int keep_alive);
	template <class FUNC, class FACTORY>
	bool RegisterHandler(const char* uri, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher>::GetInstance().RegisterHandler(uri, func, factory);
	}

	bool SendMsg(std::shared_ptr<Session> session, const Response& res);
}}

#define GAMNET_BIND_HTTP_HANDLER(uri, class_type, func) \
	static bool Http_##class_type##_##func = Gamnet::Http::RegisterHandler( \
			uri, \
			&class_type::func, \
			new Gamnet::Network::HandlerStatic<class_type>() \
	)

#endif /* HTTPSERVER_H_ */
