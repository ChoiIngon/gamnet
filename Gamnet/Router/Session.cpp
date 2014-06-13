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

namespace Gamnet { namespace Router {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>();

Session::Session() : Network::Session() {}

Session::~Session() {}

void Session::Connect(const char* host, int port, int timeout)
{
	boost::asio::ip::tcp::resolver resolver_(io_service_);
	boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({host, Format(port).c_str()}));

	auto self = std::static_pointer_cast<Session>(shared_from_this());
	socket_.async_connect(endpoint_,
		strand_.wrap([self](const boost::system::error_code& ec){
			self->timer_.Cancel();
			if(false == self->socket_.is_open())
			{
				return;
			}
			else if(ec)
			{
				self->OnError(ec.value());
			}
			else
			{
				self->sessionKey_ = ++Network::IListener::uniqueSessionKey_;
				self->readBuffer_ = Network::Packet::Create();
				self->listener_->sessionManager_.AddSession(self->sessionKey_, self);
				self->_read_start();
				self->OnConnect();
			}
		})
	);

	if(0 != timeout)
	{
		timer_.SetTimer(timeout*1000, [this]() {
			Log::Write(GAMNET_WRN, "connect timeout(session_key:", sessionKey_, ")");
			OnError(ETIMEDOUT);
		});
	}
}

void Session::OnConnect()
{
	watingSessionManager_.Clear();

	Log::Write(GAMNET_INF, "Router(ip:", socket_.remote_endpoint().address().to_string(), "):connect success");
	MsgRouter_SetAddress_Req req;
	req.tLocalAddr = Singleton<RouterListener>().localAddr_;
	if(false == Network::SendMsg(shared_from_this(), req))
	{
		Log::Write(GAMNET_ERR, "Router(id:", socket_.remote_endpoint().address().to_string(), "):send SetAddress_Req(service_name:", req.tLocalAddr.service_name.c_str(), ", id:", req.tLocalAddr.id, ")");
	};
	Log::Write(GAMNET_INF, "Router(id:", socket_.remote_endpoint().address().to_string(), "):send SetAddress_Req(service_name:", req.tLocalAddr.service_name.c_str(), ", id:", req.tLocalAddr.id, ")");
}

void Session::OnClose()
{
	Log::Write(GAMNET_INF, "Router(id:", socket_.remote_endpoint().address().to_string(), "):server down(service_name:", addr.service_name, ", id:", addr.id, ")");
	watingSessionManager_.Clear();
}

}} /* namespace Gamnet */
