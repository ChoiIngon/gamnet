#include "Dispatcher.h"

namespace Gamnet { namespace Network { namespace Http {

Dispatcher::Dispatcher() {
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::string& uri, const std::map<std::string, std::string>& param)
{
	const std::shared_ptr<Session> session = std::static_pointer_cast<Session>(link->session);
	auto itr = mapHandlerFunction_.find(uri);
	if(itr == mapHandlerFunction_.end())
	{
		LOG(GAMNET_ERR, "can't find handler function(uri:", uri, ", link_key:", link->link_key,")");
		Response res;
		res.nErrorCode = 404;
		res.sBodyContext = "404 Not found";
		session->Send(res);
		link->OnError(EINVAL);
		return ;
	}

	const HandlerFunction& handler_function = itr->second;
	std::shared_ptr<Network::IHandler> handler = handler_function.factory_->GetHandler(&session->handler_container, 0);
	if(NULL == handler)
	{
		Log::Write(GAMNET_ERR, "can't find handler function(uri:", uri, ", link_key:", link->link_key,")");
		Response res;
		res.nErrorCode = 404;
		res.sBodyContext = "404 Not found";
		session->Send(res);
		link->OnError(EINVAL);
		return;
	}
	try {
		handler_function.function_(handler, session, param);
	}
	catch (const std::exception& e)
	{
		Log::Write(Log::Logger::LOG_LEVEL_ERR, "unhandled exception occurred(reason:", e.what(), ")");
	}
}

}}}
