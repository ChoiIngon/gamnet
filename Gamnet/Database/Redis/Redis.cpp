#include "Redis.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace Gamnet { namespace Database { namespace Redis {

	void ReadXml(const char* xml_path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

		auto database_ = ptree_.get_child("server");
		for (auto elmt : database_)
		{
			if ("redis" != elmt.first)
			{
				continue;
			}
			int id = elmt.second.get<int>("<xmlattr>.id");
			int port = elmt.second.get<int>("<xmlattr>.port");
			const std::string host = elmt.second.get<std::string>("<xmlattr>.host");
			if (false == Connect(id, host.c_str(), port))
			{
				throw Exception(GAMNET_ERRNO(ErrorCode::ConnectFailError), "database connect  fail(id:", id, ", host:", host, ", port:", port, ")");
			}
		}
	}
	
	bool Connect(int db_type, const char* host, int port)
	{
		Connection::ConnectionInfo connInfo;
		connInfo.host = host;
		connInfo.port = port;
		return Singleton<ConnectionPool<Connection, ResultSet>>::GetInstance().Connect(db_type, connInfo);
	}

	ResultSet Execute(int db_type, const std::string& query)
	{
		return Singleton<ConnectionPool<Connection, ResultSet>>::GetInstance().Execute(db_type, query);
	}
} } }
