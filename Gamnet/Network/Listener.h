/*
 * Listener.h
 *
 *  Created on: Jul 4, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_LISTENER_H_
#define GAMNET_NETWORK_LISTENER_H_

#include <error.h>
#include <memory>
#include <atomic>
#include <boost/bind.hpp>
#include "../Library/Pool.h"
#include "SessionManager.h"
#include "Dispatcher.h"

namespace Gamnet { namespace Network {

class IListener
{
public :
	static std::atomic_ullong uniqueSessionKey_;
	SessionManager sessionManager_;
	IListener() {}
	virtual ~IListener() {}
	virtual void OnRecvMsg(std::shared_ptr<Session> pSession, std::shared_ptr<Packet> pPacket) = 0;
	virtual void OnClose(std::shared_ptr<Session> pSession) = 0;
};

template <class SESSION_T>
class Listener : public IListener
{
	bool bCanAccpet_;
	std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
protected :
	std::mutex lock_;
	Pool<SESSION_T, std::mutex, Session::Init> sessionPool_;

public :
	Listener() : bCanAccpet_(true), sessionPool_()
	{
	}
	virtual ~Listener()
	{
	}

	void Init(int port, int max_session, int keepAliveTime)
	{
		if(false == sessionManager_.Init(keepAliveTime))
		{
			throw Exception("sessionManager_ init fail");
		}
		sessionPool_.Capacity(max_session);
		acceptor_ =
				std::shared_ptr<boost::asio::ip::tcp::acceptor>(
					new boost::asio::ip::tcp::acceptor(
						Singleton<boost::asio::io_service>(),
						boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)
					)
				);

		_accept_start();
	}
	virtual void OnRecvMsg(std::shared_ptr<Session> session, std::shared_ptr<Packet> packet)
	{
		Singleton<Dispatcher<SESSION_T>>().OnRecvMsg(std::static_pointer_cast<SESSION_T>(session), packet);
	}
	virtual void OnClose(std::shared_ptr<Session> session)
	{
		sessionManager_.DelSession(session->sessionKey_, session);
		if(false == bCanAccpet_)
		{
			std::lock_guard<std::mutex> lo(lock_);
			if(false == bCanAccpet_)
			{
				if(0 < sessionPool_.Available())
				{
					bCanAccpet_ = true;
					_accept_start();
				}
			}
		}
	}

	template <class FUNC, class HANDLER>
	bool RegisterHandler(unsigned int msg_id, FUNC func, std::shared_ptr<HandlerCreate<HANDLER>> factory)
	{
		return Singleton<Dispatcher<SESSION_T>>().RegisterHandler(msg_id, func, factory);
	}
	template <class FUNC, class HANDLER>
	bool RegisterHandler(unsigned int msg_id, FUNC func, std::shared_ptr<HandlerStatic<HANDLER>> factory)
	{
		return Singleton<Dispatcher<SESSION_T>>().RegisterHandler(msg_id, func, factory);
	}
	template <class FUNC, class HANDLER>
	bool RegisterHandler(unsigned int msg_id, FUNC func, std::shared_ptr<HandlerFind<HANDLER>> factory)
	{
		return Singleton<Dispatcher<SESSION_T>>().RegisterHandler(msg_id, func, factory);
	}

private :
	void _accept_handler(std::shared_ptr<SESSION_T> session, const boost::system::error_code& error)
	{
		if(0 == error)
		{
			session->sessionKey_ = ++IListener::uniqueSessionKey_;
			session->listener_ = this;
			sessionManager_.AddSession(session->sessionKey_, session);
			session->_read_start();
		}

		_accept_start();
	}
	void _accept_start()
	{
		std::shared_ptr<SESSION_T> session = sessionPool_.Create();
		if(NULL == session)
		{
			Log::Write(GAMNET_WRN, "can not create any more session(max:", sessionPool_.Capacity(), ", current:", sessionManager_.Size(), ")");
			std::lock_guard<std::mutex> lo(lock_);
			bCanAccpet_ = false;
			return;
		}
		acceptor_->async_accept(session->socket_, session->strand_.wrap(boost::bind(
				&Listener<SESSION_T>::_accept_handler,
				this, session,
				boost::asio::placeholders::error
		)));
	}
};

}}
#endif /* LISTENER_H_ */