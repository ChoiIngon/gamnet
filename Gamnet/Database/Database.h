/*
 * Database.h
 *
 *  Created on: Jul 24, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_DATABASE_H_
#define GAMNET_DATABASE_DATABASE_H_

#include "DatabaseImpl.h"
#include "Transaction.h"
#include "../Library/String.h"
namespace Gamnet { namespace Database {

void ReadXml(const char* xml_path);
bool 	  Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db);
ResultSet Execute(int db_type, const std::string& query);
//void 	  AExecute(int db_type, const std::string& query, std::function<void(ResultSet)> callback);

template <class... ARGS>
ResultSet Execute(int db_type, ARGS... args)
{
	const std::string query = Format(args...);
	return Execute(db_type, query);
}

}}

#endif /* DATABASE_H_ */
