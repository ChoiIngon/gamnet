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
	virtual void OnAccept(std::shared_ptr<Session> session, const boost::system::error_code& error) = 0;
	virtual void OnRecvMsg(std::shared_ptr<Session> pSession, std::shared_ptr<Packet> pPacket) = 0;
	virtual void OnClose(std::shared_ptr<Session> pSession) = 0;
};

template <class SESSION_T>
class Listener : public IListener
{
	volatile bool bCanAccpet_;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ip::tcp::endpoint endpoint_;
protected :
	std::mutex lock_;
public :
	int port_;
	Pool<SESSION_T, std::mutex, Session::Init> sessionPool_;

public :
	Listener() : bCanAccpet_(true), acceptor_(Singleton<boost::asio::io_service>::GetInstance()), port_(0), sessionPool_()
	{
	}
	virtual ~Listener()
	{
	}
	void Init(int port, int max_session, int keepAliveTime)
	{
		port_ = port;
		if(false == sessionManager_.Init(keepAliveTime))
		{
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] sessionManager_ init fail");
		}
		sessionPool_.Capacity(max_session);
		//boost::asio::ip::tcp::resolver resolver_(Singleton<boost::asio::io_service>());
		endpoint_ = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor_.open(endpoint_.protocol());
		acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_.bind(endpoint_);
		acceptor_.listen();
		_accept_start();
	}
	virtual void OnAccept(std::shared_ptr<Session> session, const boost::system::error_code& error)
	{
		if(0 == error)
		{
			session->sessionKey_ = ++IListener::uniqueSessionKey_;
			session->listener_ = this;
			sessionManager_.AddSession(session->sessionKey_, session);
			session->remote_address_ = session->socket_.remote_endpoint().address();
			session->OnAccept();
			session->AsyncRead();
		}
		_accept_start();
	}

	virtual void OnRecvMsg(std::shared_ptr<Session> session, std::shared_ptr<Packet> packet)
	{
		Singleton<Dispatcher<SESSION_T>>::GetInstance().OnRecvMsg(std::static_pointer_cast<SESSION_T>(session), packet);
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

	template <class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().RegisterHandler(msg_id, func, factory);
	}

private :
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
		acceptor_.async_accept(session->socket_, session->strand_.wrap(boost::bind(
			&IListener::OnAccept,
			this, session,
			boost::asio::placeholders::error
		)));
	}
};

}}
#endif /* LISTENER_H_ */
