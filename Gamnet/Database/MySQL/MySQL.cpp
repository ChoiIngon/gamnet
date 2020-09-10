#include "MySQL.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"
#include "../../Library/ThreadPool.h"
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

bool Connect(int db_type, const std::string& host, int port, const std::string& id, const std::string& passwd, const std::string& db, bool fail_query_log)
{
	Connection::ConnectionInfo connInfo;
	connInfo.database = db;
	connInfo.userid = id;
	connInfo.password = passwd;
	connInfo.port = port;
	connInfo.host = host;
	connInfo.db_num = db_type;
	connInfo.fail_query_log = fail_query_log;
	return Singleton<ConnectionPool<Connection>>::GetInstance().Connect(db_type, connInfo);
}

ResultSet Execute(int db_type, const std::string& query)
{
	std::shared_ptr<Connection> conn = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
	ResultSet res(conn->Execute(query));
	return res;
}

/*
void _AsyncExecute(int db_type, const std::string& query, const std::function<void(ResultSet&)>& callback)
{
	std::shared_ptr<Connection> conn = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
	conn->AsyncExecute(query, [callback](const std::shared_ptr<ResultSetImpl> impl) {
		ResultSet res(impl);
		callback(res);
	});
}
*/

std::string RealEscapeString(int db_type, const std::string& str)
{
	std::shared_ptr<Connection> conn = Singleton<ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
	return conn->RealEscapeString(str);
}

}}}


