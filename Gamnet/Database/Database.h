/*
 * Database.h
 *
 *  Created on: Jul 24, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_DATABASE_H_
#define GAMNET_DATABASE_DATABASE_H_

#include "DatabaseImpl.h"
namespace Gamnet { namespace Database {

bool 	  Connect(int db_type, const char* host, int port, const char* id, const char* passwd, const char* db);
ResultSet Execute(int db_type, const std::string& query);
void 	  AExecute(int db_type, const std::string& query, std::function<void(ResultSet)> callback);

template <class... ARGS>
ResultSet Execute(int db_type, ARGS&&... args)
{
	const std::string query = String(args...);
	return Execute(db_type, query);
}
}}

#endif /* DATABASE_H_ */
