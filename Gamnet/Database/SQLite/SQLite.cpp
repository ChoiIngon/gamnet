#include "SQLite.h"
#include "../ConnectionPool.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace Gamnet { namespace Database { namespace SQLite {
	bool Connect(int db_type, const char* db)
	{
		Connection::ConnectionInfo connInfo;
		connInfo.db_ = db;
		return Singleton<Database::ConnectionPool<Connection>>::GetInstance().Connect(db_type, connInfo);
	}

	ResultSet Execute(int db_type, const std::string& query)
	{
		std::shared_ptr<Connection> conn = Singleton<Database::ConnectionPool<Connection>>::GetInstance().GetConnection(db_type);
		ResultSet res;
		res.impl_ = conn->Execute(query);
		res.impl_->conn_ = conn;
		return res;
	}

	void ReadXml(const char* xml_path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

		auto database_ = ptree_.get_child("server");
		for (const auto& elmt : database_)
		{
			if ("sqlite" != elmt.first)
			{
				continue;
			}
			int id = elmt.second.get<int>("<xmlattr>.id");
			const std::string db = elmt.second.get<std::string>("<xmlattr>.db");
			if (false == Connect(id, db.c_str()))
			{
				throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[SQLite] fail to create database file(id:", id, ", db_name:", db);
			}
		}
	}

}}}