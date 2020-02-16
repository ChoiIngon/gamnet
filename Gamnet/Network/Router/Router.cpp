#include "Router.h"
#include "../../Library/Singleton.h"
#include "../../Log/Log.h"
#include "../../Database/Redis/Redis.h"

namespace Gamnet { namespace Network { namespace Router {

const Address& GetRouterAddress()
{
	return Singleton<LinkManager>::GetInstance().local_address;
}

void Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& connect_callback, const std::function<void(const Address& addr)>& close_callback)
{
	Singleton<LinkManager>::GetInstance().Listen(service_name, port, connect_callback, close_callback);
	LOG(INF, "Gamnet::Router listener start(port:", port, ", "
			"router_address:",
				Singleton<LinkManager>::GetInstance().local_address.service_name, ":",
				(int)Singleton<LinkManager>::GetInstance().local_address.cast_type, ":",
				Singleton<LinkManager>::GetInstance().local_address.id, ", ",
			"ip:", Network::Tcp::GetLocalAddress().to_string(), ")");
}

void Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& connect_callback, const std::function<void(const Address& addr)>& close_callback)
{
	Singleton<LinkManager>::GetInstance().Connect(host, port, timeout, connect_callback, close_callback);
}

static std::shared_ptr<Database::Redis::Subscriber> subscriber = std::static_pointer_cast<Database::Redis::Subscriber>(Gamnet::Singleton<Gamnet::Database::Redis::SubscriberManager>::GetInstance().Create());

void ReadXml(const char* xml_path, const std::function<void(const Address& addr)>& connect_callback, const std::function<void(const Address& addr)>& close_callback)
{
	subscriber->Connect("127.0.0.1", 6379, 5);
	subscriber->Subscribe("router", [](const std::string& message) {
		LOG(DEV, "message from redis:", message);
	});

	/*
	boost::property_tree::ptree ptree_;
	boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

	auto router = ptree_.get_child("server");
	for (const auto& elmt : router)
	{
		if ("router" != elmt.first)
		{
			continue;
		}
		std::string service_name = elmt.second.get<std::string>("<xmlattr>.name");
		const std::string host = elmt.second.get<std::string>("<xmlattr>.host");
		int port = elmt.second.get<int>("<xmlattr>.port");
		
		Connect(host.c_str(), port, 30, connect_callback, close_callback);
	}
	*/

	
}
}}}
