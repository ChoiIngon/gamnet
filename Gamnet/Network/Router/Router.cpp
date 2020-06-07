#include "Router.h"
#include "../../Library/Singleton.h"
#include "../../Log/Log.h"
#include "../../Database/Redis/Redis.h"
#include "RouterHandler.h"
#include "../Tcp/Dispatcher.h"
#include "../Tcp/Tcp.h"
#include "SessionManager.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace Gamnet { namespace Network { namespace Router {

const Address& GetRouterAddress()
{
	return Singleton<SessionManager>::GetInstance().local_address;
}

void Listen(const std::string& serviceName, int port, const std::function<void(const Address& addr)>& acceptHandler, const std::function<void(const Address& addr)>& closeHandler)
{
	Singleton<Tcp::Dispatcher<Session>>::GetInstance().BindHandler(MsgRouter_SetAddress_Req::MSG_ID, "MsgRouter_SetAddress_Req", &RouterHandler::Recv_SetAddress_Req, new Network::HandlerStatic<RouterHandler>());
	Singleton<Tcp::Dispatcher<Session>>::GetInstance().BindHandler(MsgRouter_SetAddress_Ans::MSG_ID, "MsgRouter_SetAddress_Ans", &RouterHandler::Recv_SetAddress_Ans, new Network::HandlerStatic<RouterHandler>());
	Singleton<Tcp::Dispatcher<Session>>::GetInstance().BindHandler(MsgRouter_SendMsg_Ntf::MSG_ID, "MsgRouter_SendMsg_Ntf", &RouterHandler::Recv_SendMsg_Ntf, new Network::HandlerStatic<RouterHandler>());
	Singleton<Tcp::Dispatcher<Session>>::GetInstance().BindHandler(MsgRouter_HeartBeat_Ntf::MSG_ID, "MsgRouter_HeartBeat_Ntf", &RouterHandler::Recv_HeartBeat_Ntf, new Network::HandlerStatic<RouterHandler>());

	Singleton<SessionManager>::GetInstance().Listen(serviceName, port, acceptHandler, closeHandler);
	LOG(INF, "[Gamnet::Router] listener start(port:", port, ", router_address:",
				Singleton<SessionManager>::GetInstance().local_address.service_name, ":",
				(int)Singleton<SessionManager>::GetInstance().local_address.cast_type, ":",
				Singleton<SessionManager>::GetInstance().local_address.id, ", ",
			"ip:", Network::Tcp::GetLocalAddress().to_string(), ")");
}

void Connect(const std::string& host, int port, int timeout)
{
	Singleton<SessionManager>::GetInstance().Connect(host, port, timeout);
}

void ReadXml(const std::string& path, const std::function<void(const Address& addr)>& connectHandler, const std::function<void(const Address& addr)>& closeHandler)
{
	boost::property_tree::ptree ptree_;
	try {
		boost::property_tree::xml_parser::read_xml(path, ptree_);
	}
	catch (const boost::property_tree::xml_parser_error& e)
	{
		std::cerr << "[Gamnet::Tcp] config file not found error(path:" << path << ")" << std::endl;
		return;
	}

	try {
		auto router = ptree_.get_child("server.router");
		std::string service_name = router.get<std::string>("<xmlattr>.name");
		int port = router.get<int>("<xmlattr>.port");

		Listen(service_name.c_str(), port, connectHandler, closeHandler);
		for (const auto& remote : router)
		{
			if ("remote" != remote.first)
			{
				continue;
			}

			const std::string remote_host = remote.second.get<std::string>("<xmlattr>.host");
			int remote_port = remote.second.get<int>("<xmlattr>.port");
			Connect(remote_host.c_str(), remote_port, 5);
		}

		try {
			auto farm = ptree_.get_child("server.farm");
			std::string farm_host = farm.get<std::string>("<xmlattr>.host");
			int farm_port = farm.get<int>("<xmlattr>.port");

			Database::Redis::Connect(-1, farm_host.c_str(), farm_port);
			Database::Redis::Subscribe(-1, "__router__", [=](const std::string& message)
			{
				Json::Reader reader;
				Json::Value router_info;
				if (false == reader.parse(message, router_info))
				{
					LOG(ERR, "router message parse failed(message:", message, ")");
					return;
				}

				if (true == router_info["host"].isNull())
				{
					LOG(ERR, "router message format error(\'host\' is null)");
					return;
				}

				if (true == router_info["port"].isNull())
				{
					LOG(ERR, "router message format error(\'port\' is null)");
					return;
				}
				Connect(router_info["host"].asString().c_str(), router_info["port"].asInt(), 5);
			});

			Json::Value router_info;
			router_info["host"] = Network::Tcp::GetLocalAddress().to_v4().to_string();
			router_info["port"] = port;
			Database::Redis::Publish(-1, "__router__", router_info);
		}
		catch(boost::property_tree::ptree_bad_path& e)
		{
			// do nothing
		}
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		std::cerr << "[Gamnet::Router] " << e.what() << std::endl;
		throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, e.what());
	}
}
}}}
