/*
 * Dispatcher.h
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_ROUTER_DISPATCHER_H_
#define GAMNET_NETWORK_ROUTER_DISPATCHER_H_

#include "../HandlerFactory.h"
#include "../Tcp/Dispatcher.h"
#include "../Tcp/Packet.h"
#include "../../Library/Time/Timer.h"
#include "MsgRouter.h"
#include "Session.h"
#include <functional>

namespace Gamnet { namespace Network { namespace Router {
	class IHandlerFunctor
	{
	public:
		IHandlerFunctor(const std::string& name, IHandlerFactory* factory) : name(name), factory(factory) {}
		
		virtual void OnReceive(const Address&, const std::shared_ptr<Tcp::Packet>&) = 0;

		const std::string name;
	protected:
		IHandlerFactory* const factory;
	};

	template <class MsgType>
	class HandlerFunctor : public IHandlerFunctor
	{
	public:
		typedef MsgType MsgType;
		typedef void(IHandler::* FunctionType)(const Address&, const MsgType&);

		template<class FactoryType>
		HandlerFunctor(const std::string& name, FactoryType* factory, FunctionType function) : IHandlerFunctor(name, factory), function(function) {}

		virtual void OnReceive(const Address& address, const std::shared_ptr<Tcp::Packet>& packet) override
		{
			std::shared_ptr<IHandler> handler = factory->GetHandler();
			if (nullptr == handler)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidHandlerError, "can't create message handler instance(name:", name, ", msg_id:", MsgType::MSG_ID, ")");
			}

			MsgType msg;
			if (false == Tcp::Packet::Load(msg, packet))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "message load fail(name:", name, ", msg_id:", MsgType::MSG_ID, ")");
			}

			function(handler, address, msg);
		}
	private:
		std::function<void(const std::shared_ptr<IHandler>&, const Router::Address&, const MsgType&)> function;
	};

	template <>
	class HandlerFunctor<std::shared_ptr<Tcp::Packet>> : public IHandlerFunctor
	{
	public:
		typedef std::shared_ptr<Tcp::Packet> MsgType;
		typedef void(IHandler::* FunctionType)(const Address&, const std::shared_ptr<Tcp::Packet>&);

		template<class FactoryType>
		HandlerFunctor(const std::string& name, FactoryType* factory, FunctionType function) : IHandlerFunctor(name, factory), function(function) {}

		virtual void OnReceive(const Address& address, const std::shared_ptr<Tcp::Packet>& packet) override
		{
			std::shared_ptr<IHandler> handler = factory->GetHandler();
			if (nullptr == handler)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidHandlerError, "can't create message handler instance(name:", name, ")");
			}

			function(handler, address, packet);
		}
	private:
		std::function<void(const std::shared_ptr<IHandler>&, const Router::Address&, const std::shared_ptr<Tcp::Packet>&)> function;
	};

class Dispatcher
{
public :
	std::map<uint32_t, std::shared_ptr<IHandlerFunctor>> handler_functors;
	
public:
	Dispatcher();
	~Dispatcher();

	bool BindHandler(unsigned int msg_id, const std::shared_ptr<IHandlerFunctor>& handlerFunctor)
	{
		if(false == handler_functors.insert(std::make_pair(msg_id, handlerFunctor)).second)
		{
			throw GAMNET_EXCEPTION(ErrorCode::DuplicateMessageIDError, "duplicate handler(msg_id:", msg_id, ")");
		}
		return true;
	}

	void OnReceive(const std::shared_ptr<Session>& session, const std::shared_ptr<Network::Tcp::Packet>& packet);
};
} /* namespace Router */
} /* namespace Gamnet */
}

#endif /* DISPATCHER_H_ */
