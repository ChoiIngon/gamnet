#include "HttpServer.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace Gamnet { namespace Network { namespace Http {

	static Tcp::Acceptor acceptor;

	void Listen(int port, int accept_queue_size)
	{
		acceptor.Listen(port, 1024);
		LOG(GAMNET_INF, "Gamnet::Http listener start(port:", port, ")");
	}

	void ReadXml(const char* xml_path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

		int port = ptree_.get<int>("server.http.<xmlattr>.port");
		Listen(port);
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
}}}


