#ifndef GAMNET_NETWORK_COROUTINE_H
#define GAMNET_NETWORK_COROUTINE_H

#include <boost/coroutine2/all.hpp>

namespace Gamnet { namespace Network {

struct IHandler;
class Coroutine
{
	boost::coroutines2::coroutine<void>::pull_type pull_type;
	boost::coroutines2::coroutine<void>::push_type* push_type;
	std::shared_ptr<IHandler> handler;
public:
	Coroutine(std::shared_ptr<IHandler>& handler);
	virtual ~Coroutine();
	std::function<void()> coroutine;
	void start();
	void resume();
	void yield();
private:
	void Init(boost::coroutines2::coroutine<void>::push_type& push);
};
}}
#endif // _DEBUG
