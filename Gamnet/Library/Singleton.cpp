#include "Singleton.h"
#include "Time/Timer.h"

namespace Gamnet {

void SingletonInitHelper::Init()
{
	for (auto& itr : mapInitFuncInfos)
	{
		CallInitFunc(itr.second.name);
	}
}


void SingletonInitHelper::CallInitFunc(const std::string& name)
{
	auto itr = mapInitFuncInfos.find(name);
	if (mapInitFuncInfos.end() == itr)
	{
		LOG(ERR, "undefined init function(name:", name, ")");
		throw Exception(0, "undefined init function(name:", name, ")");
	}

	const InitFuncInfo& info = itr->second;
	if (false == setInitCompleted.insert(info.name).second)
	{
		return;
	}

	LOG(GAMNET_INF, "[Gamnet::Singleton<", name, ">] start initialization...");
	Time::ElapseTimer elapse;
	info.func();
	LOG(GAMNET_INF, "[Gamnet::Singleton<", name, ">] complete initialization...(", elapse.Count(), "ms)");
}

}