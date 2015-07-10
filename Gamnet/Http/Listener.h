/*
 * Listener.h
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include "Session.h"

namespace Gamnet {
namespace Http {

class Listener : public Network::IListener
{
	volatile bool bCanAccpet_;
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ip::tcp::endpoint endpoint_;
	int port_;
protected :
	std::mutex lock_;
	Pool<Session, std::mutex, Session::Init> sessionPool_;

public :
	Listener();
	virtual ~Listener();

	void Init(int port, int max_session, int keepAliveTime)
	{
		if(false == sessionManager_.Init(keepAliveTime))
		{
#ifdef _WIN32
			throw Exception(0, "[", __FILE__, ":", __FUNCTION__, "@", __LINE__, "] sessionManager_ init fail");
#else
			throw Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] sessionManager_ init fail");
#endif
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
	void OnAccept(std::shared_ptr<Network::Session> session, const boost::system::error_code& error)
	{
		if(0 == error)
		{
			session->sessionKey_ = ++IListener::uniqueSessionKey_;
			session->listener_ = this;
			try {
				session->remote_address_ = session->socket_.remote_endpoint().address();
				sessionManager_.AddSession(session->sessionKey_, session);
				session->OnAccept();
				session->AsyncRead();
			}
			catch(const boost::system::system_error& e)
			{
				Log::Write(GAMNET_ERR, "fail to accept(session_key:", session->sessionKey_, ", errno:", errno, ", errstr:", e.what(), ")");
			}
		}
		_accept_start();
	}
	virtual void OnRecvMsg(std::shared_ptr<Network::Session> session, std::shared_ptr<Network::Packet> packet)
	{
	}
	virtual void OnClose(std::shared_ptr<Network::Session> session)
	{
		sessionManager_.DelSession(session->sessionKey_, session);
	}

private :
	void _accept_start()
	{
		std::shared_ptr<Session> session = sessionPool_.Create();
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

} /* namespace Http */
} /* namespace Gamnet */
#endif /* LISTENER_H_ */
