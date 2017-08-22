#ifndef GAMNET_NETWORK_HTTP_DISPATCHER_H_
#define GAMNET_NETWORK_HTTP_DISPATCHER_H_

#include "../Tcp/Tcp.h"
#include "../HandlerFactory.h"
#include "Session.h"

namespace Gamnet { namespace Network { namespace Http {

class Dispatcher {
public:
	typedef void(Network::IHandler::*function_type)(const std::shared_ptr<Session>&, const std::map<std::string, std::string>&);
	struct HandlerFunction
	{
		template<class FACTORY, class FUNCTION>
		HandlerFunction(FACTORY* factory, FUNCTION function) : factory_(factory), function_(function) {}
		Network::IHandlerFactory* factory_;
		std::function<void(const std::shared_ptr<Network::IHandler>&, const std::shared_ptr<Session>&, const std::map<std::string, std::string>&)> function_;
	};
	std::map<std::string, HandlerFunction> mapHandlerFunction_;

	Dispatcher();
	virtual ~Dispatcher();

	template <class FUNC, class FACTORY>
	bool RegisterHandler(const char* uri, FUNC func, FACTORY factory)
	{
		HandlerFunction handlerFunction(factory, static_cast<function_type>(func));
		if(false == mapHandlerFunction_.insert(std::make_pair(uri, handlerFunction)).second)
		{
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] duplicate handler(uri:", uri, ")");
		}
		return true;
	}

	void OnRecvMsg(const std::shared_ptr<Network::Link>& link, const std::string& uri, const std::map<std::string, std::string>& param);
};

}}}
#endif /* DISPATCHER_H_ */
