#include "Coroutine.h"
#include "Handler.h"

namespace Gamnet { namespace Network {
	Coroutine::Coroutine(std::shared_ptr<IHandler>& handler) 
		: pull_type(std::bind(&Coroutine::Init, this, std::placeholders::_1))
		, push_type(nullptr)
		, handler(handler)
	{
		handler->coroutine = this;
	}

	Coroutine::~Coroutine()
	{
		;
	}

	void Coroutine::start()
	{
		pull_type();
	}

	void Coroutine::resume()
	{
		pull_type();
	}

	void Coroutine::yield()
	{
		(*this->push_type)();
	}

	void Coroutine::Init(boost::coroutines2::coroutine<void>::push_type& push)
	{
		push();
		push_type = &push;
		coroutine();
	}
}}