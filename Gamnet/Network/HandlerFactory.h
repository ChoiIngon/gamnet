#ifndef GAMNET_NETWORK_HANDLERFACTORY_H_
#define GAMNET_NETWORK_HANDLERFACTORY_H_

#include <memory>
#include <stdint.h>
#include "../Library/Debugs.h"
#include "../Library/Pool.h"
#include "HandlerContainer.h"
namespace Gamnet { namespace Network { 

struct IHandler;
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
	virtual std::shared_ptr<IHandler> GetHandler(HandlerContainer*, uint32_t) = 0;
};

template <class T>
struct HandlerCreate : public IHandlerFactory
{
	GAMNET_WHERE(T, IHandler);

	Pool<T, std::mutex> pool_;
	HandlerCreate() : pool_(65535)
	{
	}
	virtual ~HandlerCreate() {}
	virtual HANDLER_FACTORY_TYPE GetFactoryType() { return IHandlerFactory::HANDLER_FACTORY_CREATE; }
	virtual std::shared_ptr<IHandler> GetHandler(HandlerContainer*, uint32_t)
	{
		return pool_.Create();
	}
};

template <class T>
struct HandlerStatic : public IHandlerFactory
{
	GAMNET_WHERE(T, IHandler);

	std::shared_ptr<T> _handler;
	HandlerStatic() : _handler(std::shared_ptr<T>(new T()))
	{
	}
	virtual ~HandlerStatic() {}
	virtual HANDLER_FACTORY_TYPE GetFactoryType() { return IHandlerFactory::HANDLER_FACTORY_STATIC; }
	virtual std::shared_ptr<IHandler> GetHandler(HandlerContainer*, uint32_t)
	{
		return _handler;
	}
};

template <class T>
struct HandlerFind : public IHandlerFactory
{
	GAMNET_WHERE(T, IHandler);

	HandlerFind() {};
	virtual ~HandlerFind(){};
	virtual HANDLER_FACTORY_TYPE GetFactoryType() { return IHandlerFactory::HANDLER_FACTORY_FIND; }
	virtual std::shared_ptr<IHandler> GetHandler(HandlerContainer* container, uint32_t msg_seq)
	{
		if(NULL == container)
		{
			return NULL;
		}
		return container->Find(msg_seq);
	}
};

}} /* namespace Gamnet */
#endif /* HANDLERFACTORY_H_ */
