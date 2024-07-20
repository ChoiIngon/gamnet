#include "Dispatcher.h"
#include "Session.h"

namespace Gamnet { namespace Network { namespace Http {

Dispatcher::Dispatcher() {
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::OnReceive(const std::shared_ptr<Session>& session, const std::string& uri, const Request& request)
{
	auto itr = handler_functors.find(uri);
	if(handler_functors.end() == itr)
	{
		LOG(ERR, "can't find handler function(uri:", uri, ")");
		Response res;
		res.error_code = 404;
		res.context = "404 Not found";
		session->Send(res);
		return ;
	}

	const HandlerFunctor& handlerFunctor = itr->second;
	std::shared_ptr<Network::IHandler> handler = handlerFunctor.factory_->GetHandler();
	if(nullptr == handler)
	{
		LOG(ERR, "can't find handler function(uri:", uri, ")");
		Response res;
		res.error_code = 404;
		res.context = "404 Not found";
		session->Send(res);
		return;
	}
	try {
		handlerFunctor.function_(handler, session, request);
	}
	catch (const std::exception& e)
	{
		Log::Write(Log::LOG_LEVEL_TYPE::LOG_LEVEL_ERR, "[unhandled exception occurred(reason:", e.what(), ")");
	}
}

}}}
