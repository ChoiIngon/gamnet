#include "Gamnet.h"
#include "Library/Debugs.h"
#include <boost/thread.hpp>

namespace Gamnet {
	static boost::asio::io_context& io_context = Singleton<boost::asio::io_context>::GetInstance();

void Run(int thread_count)
{
	SingletonInitHelper::GetInstance().Init();

	Log::Write(GAMNET_INF, "[Gamnet] server starts..");
    boost::thread_group threads;
	for(int i=0; i<thread_count; i++)
	{
		threads.create_thread([&] {
            io_context.run();
        });
	}

	threads.join_all();
}

}



