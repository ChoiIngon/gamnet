#include "HttpServer.h"
#include <set>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "../../Library/Pool.h"

namespace Gamnet { namespace Network { namespace Http {
	static std::set<std::shared_ptr<Tcp::Acceptor>> acceptors;
	
	Pool<Session, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> session_pool;
	static void OnAccept(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
	{
		std::shared_ptr<Session> session = session_pool.Create();
		session->socket = socket;
		session->AsyncRead();
	}

	void Listen(int port, int accept_queue_size)
	{
		std::shared_ptr<Tcp::Acceptor> acceptor = std::make_shared<Tcp::Acceptor>();
		if(false == acceptors.insert(acceptor).second)
		{
#ifdef _WIN32
			MessageBoxA(nullptr, Format("[Gamnet::Netowrk::Http] already bind(port:", port, ")").c_str(), "Already Bind", MB_ICONWARNING);
#endif
			throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "[Gamnet::Netowrk::Http] already bind(port:", port, ")");

		}
		acceptor->accept_handler = &OnAccept;
		acceptor->Listen(port, accept_queue_size);
		LOG(GAMNET_INF, "[Gamnet::Netowrk::Http] listener start(port:", port, ")");
	}

	void ReadXml(const std::string& xml_path)
	{
		boost::property_tree::ptree ptree_;
		try {
			boost::property_tree::xml_parser::read_xml(xml_path, ptree_);
		}
		catch (const boost::property_tree::xml_parser_error& e)
		{
			throw Exception(ErrorCode::FileNotFound, e.what());
		}

		auto optional = ptree_.get_child_optional("server.http");
		if (false == (bool)optional)
		{
			return;
		}

		try {
			int port = ptree_.get<int>("server.http.<xmlattr>.port");
			Listen(port);
		}
		catch (const boost::property_tree::ptree_bad_path& e)
		{
			throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, e.what());
		}
	}

	void SendMsg(const std::shared_ptr<Session>& session, const Response& res)
	{
		if(nullptr == session)
		{
			LOG(ERR, "invalid session");
			return;
		}
		session->Send(res);
	}

	Json::Value State()
	{
		Json::Value root;
		root["name"] = "Gamnet::Network::Http";
		for(const std::shared_ptr<Tcp::Acceptor>& acceptor : acceptors)
		{
			root["acceptor"].append(acceptor->State());
		}
		return root;
	}
}}}


