/*
 * HttpServer.h
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HTTP_HTTPSERVER_H_
#define GAMNET_NETWORK_HTTP_HTTPSERVER_H_

#include "../Tcp/Acceptor.h"
#include "../../Library/Json/json.h"
#include "../../Library/Singleton.h"
#include "Dispatcher.h"
#include "HttpClient.h"
#include "Response.h"

namespace Gamnet { namespace Network { namespace Http {
	void Listen(int port, int accept_queue_size=1024);
	void ReadXml(const std::string& xml_path);
	template <class FUNC, class FACTORY>
	bool BindHandler(const char* uri, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher>::GetInstance().BindHandler(uri, func, factory);
	}
	void SendMsg(const std::shared_ptr<Session>& session, const Response& res);
	Json::Value State();
}}}

#define GAMNET_BIND_HTTP_HANDLER(uri, class_type, func) \
	static bool Http_##class_type##_##func = Gamnet::Network::Http::BindHandler( \
			uri, \
			&class_type::func, \
			new Gamnet::Network::HandlerStatic<class_type>() \
	)

#endif /* HTTPSERVER_H_ */
