#ifndef _GAMNET_DATABASE_REDIS_SUBSCRIBER_H_
#define _GAMNET_DATABASE_REDIS_SUBSCRIBER_H_

#include "../../Network/Session.h"
#include "../../Network/Tcp/Connector.h"
#include "../../Library/Delegate.h"
#include "../../Library/Time/Time.h"
#include "../../Library/Exception.h"
#include "../../Library/Buffer.h"
#include "../../Library/Json/json.h"
#include "../../Log/Log.h"

import Gamnet.Singleton;
import Gamnet.String;

namespace Gamnet { namespace Database { namespace Redis {
	class Subscriber : public Network::Session
	{
		std::mutex lock;
		std::shared_ptr<Buffer> recv_buffer;
		std::map<std::string, std::function<void(const std::string& message)>> callback_functions;
		std::map<std::string, std::function<void(const Json::Value&)>> handlers;

		void OnRecv_SubscribeAns(const Json::Value& ans);
		void OnRecv_PublishReq(const Json::Value& req);
	public :
		Subscriber();
		virtual ~Subscriber();

		bool Init();
		bool Connect(const std::string& host, int port);
		void Subscribe(const std::string& channel, const std::function<void(const std::string& message)>& callback);
		void Unsubscribe(const std::string& channel);

		virtual void OnCreate() override {}
		virtual void OnAccept() override {}
		virtual void OnClose(int reason) override {}
		virtual void OnDestroy() override {}
		virtual void Close(int reason);
		
	private :
		Network::Tcp::Connector connector;
		Time::Timer reconnect_timer;
		std::string host;
		int port;

		bool Reconnect();
		void AsyncSend(const std::string& query);
		void OnRead(const std::shared_ptr<Buffer>& buffer);
		void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
		void OnError(int error);
	};
}}}

#endif
