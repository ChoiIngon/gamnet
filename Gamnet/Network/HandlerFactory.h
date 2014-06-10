/*
 * HandlerFactory.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HANDLERFACTORY_H_
#define GAMNET_NETWORK_HANDLERFACTORY_H_

#include <memory>
#include <stdint.h>
#include "../Library/Pool.h"
#include "HandlerContainer.h"
namespace Gamnet { namespace Network {

struct IHandler;
struct IHandlerFactory
{
	IHandlerFactory() {}
	virtual ~IHandlerFactory() {}
	virtual std::shared_ptr<IHandler> GetHandler(HandlerContainer&, uint32_t) = 0;
};

template <class T>
struct HandlerCreate : public IHandlerFactory
{
	Pool<T, std::mutex> pool_;
	HandlerCreate() : pool_(4096)
	{
	}
	virtual ~HandlerCreate() {}
	virtual std::shared_ptr<IHandler> GetHandler(HandlerContainer&, uint32_t)
	{
		std::shared_ptr<T> handler = pool_.Create();
		return handler;
	}
};

template <class T>
struct HandlerStatic : public IHandlerFactory
{
	std::shared_ptr<T> _handler;
	HandlerStatic() : _handler(NULL)
	{
	}
	virtual ~HandlerStatic() {}
	virtual std::shared_ptr<IHandler> GetHandler(HandlerContainer&, uint32_t)
	{
		if(NULL == _handler)
		{
			_handler = std::shared_ptr<T>(new T());
		}

		return _handler;
	}
};

template <class T>
struct HandlerFind : public IHandlerFactory
{
	HandlerFind() {};
	virtual ~HandlerFind(){};
	virtual std::shared_ptr<IHandler> GetHandler(HandlerContainer& container, uint32_t msg_seq)
	{
		return container.Find(msg_seq);
	}
};

}} /* namespace Gamnet */
#endif /* HANDLERFACTORY_H_ */
