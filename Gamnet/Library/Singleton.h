/*
 * Single.h
 *
 *  Created on: 2012. 8. 13.
 *      Author: jjaehuny
 */

#ifndef __GAMNET_LIB_SINGLETON_H_
#define __GAMNET_LIB_SINGLETON_H_

#include <functional>
#include <set>
#include <map>
#include <string>
#include "Exception.h"
#include "../Log/Log.h"

namespace Gamnet {

struct SingletonInitHelper
{
	struct InitFuncInfo
	{
		std::string name;
		std::function<void()> func;
	};

	std::set<std::string> setInitCompleted;
	std::map<std::string, InitFuncInfo> mapInitFuncInfos;
	static SingletonInitHelper& GetInstance()
	{
		static SingletonInitHelper self;
		return self;
	}
	template<class INIT_FUNC>
	bool RegisterInitFunction(const std::string& name, INIT_FUNC func)
	{
		InitFuncInfo info;
		info.name = name;
		info.func = func;
		if (false == mapInitFuncInfos.insert(std::make_pair(name, info)).second)
		{
			LOG(ERR, "duplicated init function registration(name:", info.name, ")");
			throw Exception(0, "duplicated init function registration(name:", info.name, ")");
		}
		return true;
	}
	void Init()
	{
		for(auto& itr : mapInitFuncInfos)
		{
			CallInitFunc(itr.second.name);
		}
	}

	void CallInitFunc(const std::string& name)
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
};

template<class T>
class Singleton
{
public :
	Singleton() {}
	virtual ~Singleton() {}

	static T& GetInstance()
	{
		static T self;
		return self;
	}
	
	T& operator ()()
	{
		return GetInstance();
	}
};

}

#define GAMNET_BIND_INIT_HANDLER(class_type, func_type) \
	static bool Init_##__LINE__##_##func_type = Gamnet::SingletonInitHelper::GetInstance().RegisterInitFunction(#class_type, std::bind(&class_type::func_type, &Gamnet::Singleton<class_type>::GetInstance()))

#define GAMNET_CALL_INIT_HANDLER(class_type) \
	SingletonInitHelper::GetInstance().CallInitFunc(#class_type);
#endif /* TOOLKIT_SINGLE_H_ */
