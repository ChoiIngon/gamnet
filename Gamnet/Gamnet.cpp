#include "Gamnet.h"
#include "Library/Debugs.h"


namespace Gamnet {
static boost::asio::io_context& io_context = Singleton<boost::asio::io_context>::GetInstance();

void Run(int thread_count)
{
	SingletonInitHelper::GetInstance().Init();

	Log::Write(GAMNET_INF, "[Gamnet] server starts..");
	std::vector<std::thread > threads;
	for(int i=0; i<thread_count; i++)
	{
		threads.push_back(std::thread(boost::bind(&boost::asio::io_context::run, &io_context)));
	}

	io_context.run();
}

}



