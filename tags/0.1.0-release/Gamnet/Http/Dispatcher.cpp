/*
 * Dispatcher.cpp
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#include "Dispatcher.h"

namespace Gamnet {
namespace Http {

Dispatcher::Dispatcher() {
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::OnRecvMsg(std::shared_ptr<Session> session, const std::string& uri, const std::map<std::string, std::string>& param)
{
	auto itr = mapHandlerFunction_.find(uri);
	if(itr == mapHandlerFunction_.end())
	{
		Log::Write(GAMNET_ERR, "can't find handler function(uri:", uri, ", session_key:", session->sessionKey_,")");
		Response res;
		res.nErrorCode = 404;
		session->Send(res);
		session->OnError(EINVAL);
		return ;
	}

	const HandlerFunction& handler_function = itr->second;
	std::shared_ptr<Network::IHandler> handler = handler_function.factory_->GetHandler(&session->handlerContainer_, 0);
	if(NULL == handler)
	{
		Log::Write(GAMNET_ERR, "can't find handler function(uri:", uri, ", session_key:", session->sessionKey_,")");
		Response res;
		res.nErrorCode = 404;
		session->Send(res);
		session->OnError(EINVAL);
		return;
	}
	try {
		handler_function.function_(handler, session, param);
	}
	catch (const std::exception& e)
	{
		Log::Write(GAMNET_ERR, "unhandled exception occurred(reason:", e.what(), ")");
	}
}

} /* namespace Http */
} /* namespace Gamnet */
