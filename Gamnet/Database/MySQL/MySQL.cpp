#include "MySQL.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace Gamnet { namespace Database { namespace MySQL {

void ReadXml(const char* xml_path)
{
	boost::property_tree::ptree ptree_;
	boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

	auto database_ = ptree_.get_child("server");
	for(const auto& elmt : database_)
	{
		if("mysql" != elmt.first)
		{
			continue;
		}
		int id = elmt.second.get<int>("<xmlattr>.id");
		int port = elmt.second.get<int>("<xmlattr>.port");
		const std::string host = elmt.second.get<std::string>("<xmlattr>.host");
		const std::string user = elmt.second.get<std::string>("<xmlattr>.user");
		const std::string passwd = elmt.second.get<std::string>("<xmlattr>.passwd");
		const std::string db = elmt.second.get<std::string>("<xmlattr>.db");
		bool fail_query_log = elmt.second.get<bool>("<xmlattr>.fail_query_log", false); 
		if(false == Connect(id, host.c_str(), port, user.c_str(), passwd.c_str(), db.c_str(), fail_query_log))
		{
			throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[Gamnet::Database::MySQL] database connect fail(id:", id, ", host:", host, ", port:", port, ", user:", user, ", password:", passwd, ", db_name:", db);
		}
	}
}

bool Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db, bool fail_query_log)
{
	Connection::ConnectionInfo connInfo;
	connInfo.db_ = db;
	connInfo.id_ = id;
	connInfo.passwd_ = passwd;
	connInfo.port_ = port;
	connInfo.uri_ = host;
	connInfo.db_type_ = db_type;
	connInfo.fail_query_log_ = fail_query_log;
	return Singleton<ConnectionPool<Connection>>::GetInstance().Connect(db_type, connInfo);
}

ResultSet Execute(int db_type, const std::string& query)
{
	std::shared_ptr<Connection> conn = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
	ResultSet res;
	res.impl_ = conn->Execute(query);
	return res;
}

std::string RealEscapeString(int db_type, const std::string& str)
{
	std::shared_ptr<Connection> conn = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
	return conn->RealEscapeString(str);
}

}}}


