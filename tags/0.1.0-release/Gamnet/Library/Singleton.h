/*
 * Single.h
 *
 *  Created on: 2012. 8. 13.
 *      Author: jjaehuny
 */

#ifndef __GAMNET_LIB_SINGLETON_H_
#define __GAMNET_LIB_SINGLETON_H_

#include <vector>
#include <functional>

namespace Gamnet {

struct SingletonInitHelper
{
	std::vector<std::function<void()>> vecInitFunc_;
	static SingletonInitHelper& GetInstance()
	{
		static SingletonInitHelper self;
		return self;
	}
	template<class INIT_FUNC>
	bool RegisterInitFunction(INIT_FUNC func)
	{
		vecInitFunc_.push_back(func);
		return true;
	}
	void Init()
	{
		for(auto& init_func :vecInitFunc_)
		{
			init_func();
		}
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

	template <class INIT_FUNC_PTR>
	bool RegisterInitFunction(INIT_FUNC_PTR func)
	{
		return SingletonInitHelper::GetInstance().RegisterInitFunction(std::bind(func, &GetInstance()));
	}

	T& operator ()()
	{
		return GetInstance();
	}
};

}

#define GAMNET_BIND_INIT_HANDLER(class_type, func_type) \
	bool Init_##class_type##_##func_type = Gamnet::Singleton<class_type>().RegisterInitFunction(&class_type::func_type)
#endif /* TOOLKIT_SINGLE_H_ */
