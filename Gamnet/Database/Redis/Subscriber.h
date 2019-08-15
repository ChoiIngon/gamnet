#ifndef _GAMNET_DATABASE_REDIS_SUBSCRIBER_H_
#define _GAMNET_DATABASE_REDIS_SUBSCRIBER_H_

#include <boost/asio.hpp>
#include "Connection.h"
#include "../../Library/Delegate.h"
#include "../../Library/Timer.h"
#include "../../Library/Exception.h"
#include "../../Library/Singleton.h"
#include "../../Library/Buffer.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database { namespace Redis {
	class Subscriber : public std::enable_shared_from_this<Subscriber> 
	{
		std::map<std::string, Delegate<void(const std::string& message)>> callback_functions;

		void OnRecv_SubscribeAns(const Json::Value& ans);
		void OnRecv_PublishReq(const Json::Value& req);
	public :
		Subscriber();
		virtual ~Subscriber();

		bool Connect(const char* host, int port, int timeout);
		void Close(int reason);

		void AsyncSend(const std::string& query);
		void AsyncRead();

		bool Subscribe(const std::string& channel, const std::function<void(const std::string& message)>& callback);
		void Unsubscribe(const std::string& channel);

	public :
		boost::asio::ip::tcp::socket	socket;
		boost::asio::strand				strand;
		boost::asio::ip::address 		remote_address;
		boost::asio::deadline_timer		deadline_timer;

		std::map<std::string, std::function<void(const Json::Value&)>> handlers;
		boost::asio::streambuf streambuf;
	};
}}}

#endif
