module;

#include <boost/asio.hpp>
#include <boost/thread.hpp>

export module Gamnet;

export import Gamnet.String;
export import Gamnet.Log;
export import Gamnet.Singleton;
export import Gamnet.Dump;

namespace Gamnet {

	static boost::asio::io_context& io_context = Singleton<boost::asio::io_context>::GetInstance();
	
	export void Run(int thread_count)
	{
		Singleton<SingletonInitHelper>::GetInstance().Init();
		
		Log::Write(Log::LOG_LEVEL_TYPE::LOG_LEVEL_INF, "[Gamnet] server starts..");
		
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



