/*
 * Gamnet.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_GAMNET_H_
#define GAMNET_GAMNET_H_

#include "Library/Singleton.h"
#include "Log/Log.h"
#include "Network/Network.h"
#include "Router/Router.h"
#include "Test/Test.h"
#include "Database/Database.h"

namespace Gamnet {

struct Initializer
{
	std::vector<std::function<void()>> vecInitializer_;

	template <class INIT_FUNCTION>
	void RegisterInitHandler(INIT_FUNCTION init_func)
	{
		vecInitializer_.push_back(init_func);
	}
	void Init();
};

template <class INIT_FUNCTION>
bool RegisterInitHandler(INIT_FUNCTION init_func)
{
	Singleton<Initializer>().RegisterInitHandler(init_func);
	return true;
}
void Run(int thread_count = 0);
}
#define GAMNET_BIND_INIT_HANDLER(class_type, func_type) \
	bool Init_##class_type##_##func_type = Gamnet::RegisterInitHandler(std::bind(&class_type::func_type, &(Singleton<class_type>())))
#endif /* GAMNET_H_ */
