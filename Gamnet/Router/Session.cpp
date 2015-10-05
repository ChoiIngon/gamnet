/*
 * RouterSession.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: kukuta
 */

#include "Session.h"
#include "../Network/Network.h"
#include "../Log/Log.h"
#include "RouterListener.h"
#include "RouterCaster.h"

namespace Gamnet { namespace Router {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Session::Session() : Network::Session() {
	onRouterConnect = [](const Address&) {};
	onRouterClose = [](const Address&) {};
}

Session::~Session() {}

void Session::OnAccept() {
	onRouterClose = RouterListener::onRouterClose;
}

void Session::Connect(const char* host, int port, int timeout)
{
	boost::asio::ip::tcp::resolver resolver_(io_service_);
	boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({host, Format(port).c_str()}));

	Log::Write(GAMNET_INF, "[Router] connect.....(host:", host, ", port:", port, ")");
	remote_address_ = endpoint_.address();
	auto self = std::static_pointer_cast<Session>(shared_from_this());
	socket_.async_connect(endpoint_,
		strand_.wrap([self](const boost::system::error_code& ec){
			self->timer_.Cancel();
			if(ec)
			{
				self->OnError(ec.value());
			}
			else
			{
				self->sessionKey_ = ++Network::IListener::uniqueSessionKey_;
				self->readBuffer_ = Network::Packet::Create();
				try {
					self->remote_address_ = self->socket_.remote_endpoint().address();
					self->listener_->sessionManager_.AddSession(self->sessionKey_, self);
					self->AsyncRead();
					self->OnConnect();
				}
				catch(const boost::system::system_error& e)
				{
					Log::Write(GAMNET_ERR, "fail to accept(session_key:", self->sessionKey_, ", errno:", errno, ", errstr:", e.what(), ")");
				}
			}
		})
	);

	if(0 != timeout)
	{
		timer_.SetTimer(timeout*1000, strand_.wrap([self]() {
			Log::Write(GAMNET_WRN, "connect timeout(ip:", self->remote_address_.to_string(), ")");
			self->OnError(ETIMEDOUT);
		}));
	}
}

void Session::OnConnect()
{
	watingSessionManager_.Clear();

	Log::Write(GAMNET_INF, "[Router] connect success..(remote ip:", remote_address_.to_string(), ")");
	MsgRouter_SetAddress_Req req;
	req.tLocalAddr = Singleton<RouterListener>::GetInstance().localAddr_;
	if(false == Network::SendMsg(shared_from_this(), req))
	{
		return;
	}
	Log::Write(GAMNET_INF, "[Router] send SetAddress_Req (localhost->", remote_address_.to_string(), ", service_name:", req.tLocalAddr.service_name.c_str(), ")");
}

void Session::OnClose(int reason)
{
	Log::Write(GAMNET_INF, "[Router] remote server closed(ip:", remote_address_.to_string(), ", service_name:", addr.service_name, ")");
	if("" != addr.service_name)
	{
		std::lock_guard<std::mutex> lo(RouterListener::lock);
		onRouterClose(addr);
	}
	watingSessionManager_.Clear();
	Singleton<RouterCaster>::GetInstance().UnregisterAddress(addr);
}

}} /* namespace Gamnet */
