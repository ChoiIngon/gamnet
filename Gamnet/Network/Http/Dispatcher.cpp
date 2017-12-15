#include "Dispatcher.h"

namespace Gamnet { namespace Network { namespace Http {

Dispatcher::Dispatcher() {
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::string& uri, const Request& request)
{
	const std::shared_ptr<Session> session = std::static_pointer_cast<Session>(link->session);
	auto itr = mapHandlerFunction_.find(uri);
	if(itr == mapHandlerFunction_.end())
	{
		LOG(ERR, "[link_key:", link->link_key,"] can't find handler function(uri:", uri, ")");
		Response res;
		res.error_code = 404;
		res.context = "404 Not found";
		session->Send(res);
		return ;
	}

	const HandlerFunction& handler_function = itr->second;
	std::shared_ptr<Network::IHandler> handler = handler_function.factory_->GetHandler(&session->handler_container, 0);
	if(NULL == handler)
	{
		LOG(ERR, "[link_key:", link->link_key,"] can't find handler function(uri:", uri, ")");
		Response res;
		res.error_code = 404;
		res.context = "404 Not found";
		session->Send(res);
		return;
	}
	try {
		handler_function.function_(handler, session, request);
	}
	catch (const std::exception& e)
	{
		Log::Write(Log::Logger::LOG_LEVEL_ERR, "[link_key:", link->link_key,"] unhandled exception occurred(reason:", e.what(), ")");
	}
}

}}}
