/*
 * Connection.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: kukuta
 */

#include "Connection.h"
#include <boost/algorithm/string.hpp>
#include "../Library/Exception.h"
#include "../Library/String.h"

namespace Gamnet {
namespace Database {

Connection::Connection()
{
}

Connection::~Connection()
{
	mysql_close(&conn_) ;
}

bool Connection::Connect(const ConnectionInfo& connInfo)
{
	connInfo_ = connInfo;
	if(NULL == mysql_init(&conn_)) // mysql_thread_init() automatically called
	{
		return false;
	}

	mysql_options(&conn_, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&connInfo_.timeout_);
	mysql_options(&conn_, MYSQL_OPT_RECONNECT, (const char*)&connInfo_.reconnect_);
    mysql_options(&conn_, MYSQL_SET_CHARSET_NAME, connInfo.charset_.c_str());
    mysql_options(&conn_, MYSQL_INIT_COMMAND, Format("SET NAMES '", connInfo.charset_, "';").c_str());

    if(NULL == mysql_real_connect(&conn_, connInfo.uri_.c_str(), connInfo.id_.c_str(), connInfo.passwd_.c_str(), connInfo.db_.c_str(), connInfo.port_, NULL, 0))
    {
		mysql_thread_end();
		return false;
	}
	return true;
}

std::shared_ptr<ResultSetImpl> Connection::Execute(const std::string& query)
{
	if(0 != mysql_real_query(&conn_, query.c_str(), query.length()))
	{
		throw Exception(mysql_errno(&conn_), mysql_error(&conn_), "(query:", query, ")");
	}
	std::shared_ptr<ResultSetImpl> impl(new ResultSetImpl());
	unsigned int num_fields = mysql_field_count(&conn_);
	if(0 < num_fields)
	{
		impl->res_ = mysql_store_result(&conn_) ;
		if(NULL == impl->res_)
		{
			throw Exception(mysql_errno(&conn_), mysql_error(&conn_), "(query:", query, ")");
		}

		MYSQL_FIELD *field = NULL;
		for(unsigned int i = 0; (field = mysql_fetch_field(impl->res_)); i++)
		{
			impl->mapColumnName_[field->name] = i;
		}
	}
	else
	{
		impl->affectedRowCount_ = mysql_affected_rows(&conn_) ;
		impl->lastInsertID_ = static_cast<unsigned int>(mysql_insert_id(&conn_)) ;
	}
	return impl;
}

} /* namespace Database */
} /* namespace Gamnet */
