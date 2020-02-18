#include "Test.h"

namespace Gamnet { namespace Test {
	static std::function<void()> _runFunctor = [](){};
	
	void Run(int thread_count)
	{
		_runFunctor();
	}

	void RegisterRun(std::function<void()> runFunctor)
	{
		_runFunctor = runFunctor;
	}
}}
