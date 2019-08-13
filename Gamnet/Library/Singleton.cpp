#include "Singleton.h"

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
	info.func();
	LOG(GAMNET_INF, "init complete singleton..", name);
}

}