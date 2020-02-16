#ifndef _GAMNET_DATABASE_REDIS_SUBSCRIBER_H_
#define _GAMNET_DATABASE_REDIS_SUBSCRIBER_H_

#include <boost/asio.hpp>
#include "Connection.h"
#include "../../Network/Tcp/Tcp.h"
#include "../../Library/Delegate.h"
#include "../../Library/Timer.h"
#include "../../Library/Exception.h"
#include "../../Library/Singleton.h"
#include "../../Library/Buffer.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database { namespace Redis {
	class Subscriber : public Network::Link
	{
		std::shared_ptr<Buffer> recv_buffer;
		std::map<std::string, Delegate<void(const std::string& message)>> callback_functions;

		void OnRecv_SubscribeAns(const Json::Value& ans);
		void OnRecv_PublishReq(const Json::Value& req);
	public :
		Subscriber(Network::LinkManager* linkManager);
		virtual ~Subscriber();

		bool Init();
		void AsyncSend(const std::string& query);
		void OnRead(const std::shared_ptr<Buffer>& buffer);
		bool Subscribe(const std::string& channel, const std::function<void(const std::string& message)>& callback);
		void Unsubscribe(const std::string& channel);

	public :
		std::map<std::string, std::function<void(const Json::Value&)>> handlers;
	};

	class SubscriberManager : public Network::LinkManager
	{
	public :
		SubscriberManager();
		virtual std::shared_ptr<Network::Link> Create() override;
	};
}}}

#endif
