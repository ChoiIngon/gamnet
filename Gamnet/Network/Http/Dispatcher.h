#ifndef GAMNET_NETWORK_HTTP_DISPATCHER_H_
#define GAMNET_NETWORK_HTTP_DISPATCHER_H_

#include "../HandlerFactory.h"
#include "Request.h"
#include "Session.h"

namespace Gamnet { namespace Network { namespace Http {

class Dispatcher {
public:
	typedef void(Network::IHandler::*function_type)(const std::shared_ptr<Session>&, const Request&);
	struct HandlerFunctor
	{
		template<class FACTORY, class FUNCTION>
		HandlerFunctor(FACTORY* factory, FUNCTION function) : factory_(factory), function_(function) {}
		Network::IHandlerFactory* factory_;
		std::function<void(const std::shared_ptr<Network::IHandler>&, const std::shared_ptr<Session>&, const Request&)> function_;
	};
	std::map<std::string, HandlerFunctor> handler_functors;

	Dispatcher();
	virtual ~Dispatcher();

	template <class FUNC, class FACTORY>
	bool BindHandler(const char* uri, FUNC func, FACTORY factory)
	{
		HandlerFunctor handlerFunctor(factory, static_cast<function_type>(func));
		if(false == handler_functors.insert(std::make_pair(uri, handlerFunctor)).second)
		{
#ifdef _WIN32
			MessageBoxA(nullptr, Format("duplicate handler(uri:", uri, ")").c_str(), "Duplicate Handler Error", MB_ICONWARNING);
#endif
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] duplicate handler(uri:", uri, ")");
		}
		return true;
	}

	void OnReceive(const std::shared_ptr<Session>& session, const std::string& uri, const Request& request);
};

}}}
#endif /* DISPATCHER_H_ */
