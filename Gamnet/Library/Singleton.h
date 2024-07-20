#ifndef __GAMNET_LIB_SINGLETON_H_
#define __GAMNET_LIB_SINGLETON_H_

import Gamnet.Singleton;

#define GAMNET_TOKEN_PASTE(x, y) x ## y
#define GAMNET_TOKEN_PASTE_2(x, y) GAMNET_TOKEN_PASTE(x, y)

#define GAMNET_BIND_INIT_HANDLER(class_type, func_type) \
	static bool GAMNET_TOKEN_PASTE_2(func_type, __COUNTER__) = Gamnet::Singleton<SingletonInitHelper>::GetInstance().RegisterInitFunction(#class_type, std::bind(&class_type::func_type, &Gamnet::Singleton<class_type>::GetInstance()))

#define GAMNET_CALL_INIT_HANDLER(class_type) \
	Gamnet::Singleton<SingletonInitHelper>::GetInstance().CallInitFunc(#class_type);

#endif /* __GAMNET_LIB_SINGLETON_H_ */
