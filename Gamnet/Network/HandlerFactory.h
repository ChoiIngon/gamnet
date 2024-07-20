#ifndef GAMNET_NETWORK_HANDLERFACTORY_H_
#define GAMNET_NETWORK_HANDLERFACTORY_H_

#include <memory>
#include <stdint.h>
#include "../Library/Pool.h"
#include "Handler.h"

namespace Gamnet { namespace Network { 

struct IHandlerFactory
{
	enum HANDLER_FACTORY_TYPE
	{
		HANDLER_FACTORY_CREATE,
		HANDLER_FACTORY_STATIC,
		HANDLER_FACTORY_FIND
	};
	IHandlerFactory() {}
	virtual ~IHandlerFactory() {}
	virtual HANDLER_FACTORY_TYPE GetFactoryType() = 0;
	virtual std::shared_ptr<IHandler> GetHandler() = 0;
};

template <class T>
struct HandlerCreate : public IHandlerFactory
{
	static_assert(std::is_base_of<IHandler, T>::value);

	Pool<T, std::mutex> pool_;
	HandlerCreate() : pool_(65535)
	{
	}
	virtual ~HandlerCreate() {}
	virtual HANDLER_FACTORY_TYPE GetFactoryType() { return IHandlerFactory::HANDLER_FACTORY_CREATE; }
	virtual std::shared_ptr<IHandler> GetHandler()
	{
		return pool_.Create();
	}
};

template <class T>
struct HandlerStatic : public IHandlerFactory
{
	static_assert(std::is_base_of<IHandler, T>::value);

	std::shared_ptr<T> _handler;
	HandlerStatic() : _handler(std::shared_ptr<T>(new T()))
	{
	}
	virtual ~HandlerStatic() {}
	virtual HANDLER_FACTORY_TYPE GetFactoryType() { return IHandlerFactory::HANDLER_FACTORY_STATIC; }
	virtual std::shared_ptr<IHandler> GetHandler()
	{
		return _handler;
	}
};

}} /* namespace Gamnet */
#endif /* HANDLERFACTORY_H_ */
