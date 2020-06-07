#ifndef _GAMNET_DATABASE_REDIS_SUBSCRIBER_H_
#define _GAMNET_DATABASE_REDIS_SUBSCRIBER_H_

#include <boost/asio.hpp>
#include "../../Network/Session.h"
#include "../../Network/Tcp/Connector.h"
#include "../../Library/Delegate.h"
#include "../../Library/Timer.h"
#include "../../Library/Exception.h"
#include "../../Library/Singleton.h"
#include "../../Library/Buffer.h"
#include "../../Library/Json/json.h"
#include "../../Log/Log.h"

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
		void Connect(const std::string& host, int port);
		void Subscribe(const std::string& channel, const std::function<void(const std::string& message)>& callback);
		void Unsubscribe(const std::string& channel);

		virtual void OnCreate() override {}
		virtual void OnAccept() override {}
		virtual void OnClose(int reason) override {}
		virtual void OnDestroy() override {}
	private :
		Network::Tcp::Connector connector;
		void AsyncSend(const std::string& query);
		void OnRead(const std::shared_ptr<Buffer>& buffer);
		void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
	};
}}}

#endif
