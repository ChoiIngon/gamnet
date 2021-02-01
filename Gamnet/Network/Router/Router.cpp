#include "Router.h"
#include "../../Log/Log.h"
#include "../../Library/Singleton.h"
#include "../../Database/Redis/Redis.h"
#include "../Tcp/Dispatcher.h"
#include "../Tcp/Tcp.h"
#include "RouterHandler.h"
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
	Address localRouterAddress(ROUTER_CAST_TYPE::UNI_CAST, serviceName, Network::Tcp::GetLocalAddress().to_v4().to_ulong());
	Listen(localRouterAddress, port, acceptHandler, closeHandler);
}

void Listen(const Address& localRouterAddress, int port, const std::function<void(const Address& addr)>& acceptHandler, const std::function<void(const Address& addr)>& closeHandler)
{
	IHandlerFactory* handlerFactory = new Network::HandlerStatic<RouterHandler>();
	Tcp::BindHandler<Session, MsgRouter_Connect_Req>("MsgRouter_Connect_Req", &RouterHandler::Recv_Connect_Req, handlerFactory);
	Tcp::BindHandler<Session, MsgRouter_Connect_Ans>("MsgRouter_Connect_Ans", &RouterHandler::Recv_Connect_Ans, handlerFactory);
	Tcp::BindHandler<Session, MsgRouter_RegisterAddress_Req>("MsgRouter_RegisterAddress_Req", &RouterHandler::Recv_RegisterAddress_Req, handlerFactory);
	Tcp::BindHandler<Session, MsgRouter_RegisterAddress_Ans>("MsgRouter_RegisterAddress_Ans", &RouterHandler::Recv_RegisterAddress_Ans, handlerFactory);
	Tcp::BindHandler<Session, MsgRouter_RegisterAddress_Ntf>("MsgRouter_RegisterAddress_Ntf", &RouterHandler::Recv_RegisterAddress_Ntf, handlerFactory);
	Tcp::BindHandler<Session, MsgRouter_SendMsg_Ntf>("MsgRouter_SendMsg_Ntf", &RouterHandler::Recv_SendMsg_Ntf, handlerFactory);
	Tcp::BindHandler<Session, MsgRouter_HeartBeat_Ntf>("MsgRouter_HeartBeat_Ntf", &RouterHandler::Recv_HeartBeat_Ntf, handlerFactory);

	Singleton<SessionManager>::GetInstance().Listen(localRouterAddress, port, acceptHandler, closeHandler);
	LOG(INF, "[Gamnet::Network::Router] listener start(port:", port, ", router_address:",
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
		std::cerr << "[Gamnet::Network::Tcp] config file not found error(path:" << path << ")" << std::endl;
		return;
	}

	auto optional = ptree_.get_child_optional("server.router");
	if (false == (bool)optional)
	{
		return;
	}
	try {
		auto router = ptree_.get_child("server.router");
		std::string service_name = router.get<std::string>("<xmlattr>.name");
		uint32_t id = router.get<uint32_t>("<xmlattr>.id", Network::Tcp::GetLocalAddress().to_v4().to_ulong());
		int port = router.get<int>("<xmlattr>.port");

		Address localRouterAddress(ROUTER_CAST_TYPE::UNI_CAST, service_name, id);
		Listen(localRouterAddress, port, connectHandler, closeHandler);
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
		std::cerr << "[Gamnet::Network::Router] " << e.what() << std::endl;
		throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, e.what());
	}
}

}}}
