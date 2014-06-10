/*
 * TestSession.cpp
 *
 *  Created on: Jun 7, 2014
 *      Author: kukuta
 */

#include "Session.h"
#include "Tester.h"
namespace Gamnet { namespace Test {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>();

Session::Session() : testSEQ_(0) {
}

Session::~Session() {
}

void Session::Connect(const char* host, int port)
{
	boost::asio::ip::tcp::resolver resolver_(io_service_);
	boost::asio::ip::tcp::resolver::query query_(host, String(port).c_str());
	boost::asio::ip::tcp::resolver::iterator itr = resolver_.resolve(query_);
	boost::asio::ip::tcp::resolver::iterator end;

	if(end != itr)
	{
		boost::asio::ip::tcp::endpoint endpoint_ = *itr;

		auto self = std::static_pointer_cast<Session>(shared_from_this());
		socket_.async_connect(endpoint_,strand_.wrap(boost::bind(&Tester<Session>::OnConnect, static_cast<Tester<Session>*>(self->listener_), self)));
	}
}
} }/* namespace Gamnet */
