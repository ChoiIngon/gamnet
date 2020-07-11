#include "Gamnet.h"
#include "Library/Debugs.h"


namespace Gamnet {
static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

void Run(int thread_count)
{
	SingletonInitHelper::GetInstance().Init();

	Log::Write(GAMNET_INF, "Gamnet server starts..");
	std::vector<std::thread > threads;
	for(int i=0; i<thread_count; i++)
	{
		threads.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
	}

	//Test::Run(thread_count);
	io_service_.run();
}

}



