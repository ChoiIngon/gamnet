/*
 * TestSession.cpp
 *
 *  Created on: Jun 7, 2014
 *      Author: kukuta
 */

#include "Session.h"
#include "Tester.h"
namespace Gamnet { namespace Test {

Session::Session() : testSEQ_(-1) {
}

Session::~Session() {
}

void Session::Connect(const boost::asio::ip::tcp::endpoint& endpoint)
{
	auto self = std::static_pointer_cast<Session>(shared_from_this());

	socket_.async_connect(endpoint,
		strand_.wrap([self](const boost::system::error_code& ec){
			if(false == self->socket_.is_open())
			{
				Log::Write(GAMNET_ERR, "[TEST] connect fail...");
				return;
			}
			if(ec)
			{
				LOG(GAMNET_ERR, "[TEST] ", ec.message());
				self->OnError(ec.value());
			}
			else
			{
				self->socket_.set_option(boost::asio::socket_base::linger(true, 0));
				self->sessionKey_ = ++Network::IListener::uniqueSessionKey_;
				self->readBuffer_ = Network::Packet::Create();
				self->listener_->sessionManager_.AddSession(self->sessionKey_, self);
				self->AsyncRead();
				std::bind(&Tester<Session>::OnConnect, static_cast<Tester<Session>*>(self->listener_), self)();
			}
		})
	);
}

}}/* namespace Gamnet */
