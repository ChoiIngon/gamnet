module;

#include "./Exception.h"
#include <functional>
#include <map>
#include <string>

export module Gamnet.Singleton;

import Gamnet.String;

namespace Gamnet 
{
	export template<class T>
	class Singleton
	{
	public:
		Singleton() = default;
		~Singleton() = default;

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

	export struct SingletonInitHelper
	{
		struct InitFuncInfo
		{
			std::string name;
			std::function<void()> func;
			bool initCompleted;
		};

		std::map<std::string, InitFuncInfo> mapInitFuncInfos;

		void Init()
		{
			for (auto& itr : mapInitFuncInfos)
			{
				CallInitFunc(itr.second.name);
			}
		}

		void CallInitFunc(const std::string& name)
		{
			auto itr = mapInitFuncInfos.find(name);
			if (mapInitFuncInfos.end() == itr)
			{
				throw Exception(ErrorCode::UndefinedError, "undefined init function(name:", name, ")");
			}

			InitFuncInfo& info = itr->second;
			if (true == info.initCompleted)
			{
				return;
			}

			info.func();
			info.initCompleted = true;
		}

		bool RegisterInitFunction(const std::string& name, std::function<void()>& func)
		{
			InitFuncInfo info;
			info.name = name;
			info.func = func;
			info.initCompleted = false;
			if (false == mapInitFuncInfos.insert(std::make_pair(name, info)).second)
			{
				throw Exception(ErrorCode::UndefinedError, "duplicated init function registration(name:", info.name, ")");
			}
			return true;
		}
	};
}