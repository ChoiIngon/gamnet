/*
 * Gamnet.cpp
 *
 *  Created on: Jun 9, 2014
 *      Author: kukuta
 */

#include "Gamnet.h"

namespace Gamnet {
static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

void Run(int thread_count)
{
	SingletonInitHelper::GetInstance().Init();

	std::thread handlerThread(std::bind(&Network::CreateServiceThreadPool, thread_count * 2));

	Log::Write(GAMNET_INF, "Gamnet server starts..");
	std::vector<std::thread > workers_;
	for(int i=0; i<thread_count; i++)
	{
		workers_.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
	}

	io_service_.run();
}

}



