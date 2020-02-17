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

void ReadXml(const char* xml_path, const std::function<void(const Address& addr)>& connect_callback, const std::function<void(const Address& addr)>& close_callback)
{
	boost::property_tree::ptree ptree_;
	boost::property_tree::xml_parser::read_xml(xml_path, ptree_);
	auto router = ptree_.get_child("server.router");
	std::string service_name = router.get<std::string>("<xmlattr>.name");
	int port = router.get<int>("<xmlattr>.port");

	Listen(service_name.c_str(), port, connect_callback, close_callback);
	Connect(Network::Tcp::GetLocalAddress().to_v4().to_string().c_str(), port, 5, connect_callback, close_callback);

	auto farm = router.get_child("farm");

	std::string farm_host = farm.get<std::string>("<xmlattr>.host");
	int farm_port = farm.get<int>("<xmlattr>.port");
	Database::Redis::Connect(-1, farm_host.c_str(), farm_port);
	Database::Redis::Subscribe(-1, "__router__", [=] (const std::string& message) 
	{
		Json::Reader reader;
		Json::Value router_info;
		if(false == reader.parse(message, router_info))
		{
			LOG(ERR, "router message parse failed(message:", message, ")");
			return;
		}

		if(true == router_info["host"].isNull())
		{
			LOG(ERR, "router message format error(\'host\' is null)");
			return;
		}

		if(true == router_info["port"].isNull())
		{
			LOG(ERR, "router message format error(\'port\' is null)");
			return;
		}
		Connect(router_info["host"].asString().c_str(), router_info["port"].asInt(), 5, connect_callback, close_callback);
	});

	Json::Value router_info;
	router_info["host"] = Network::Tcp::GetLocalAddress().to_v4().to_string();
	router_info["port"] = port;
	Database::Redis::Publish(-1, "__router__", router_info);
}
}}}
