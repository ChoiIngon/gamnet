/*
 * Connection.h
 *
 *  Created on: Aug 25, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_MYSQL_CONNECTION_H_
#define GAMNET_DATABASE_MYSQL_CONNECTION_H_

#include <mysql.h>
#include <string>
#include "ResultSet.h"
#include "../../Library/Pool.h"
#include "../../Log/Log.h"

namespace Gamnet::Database::MySQL 
{
	class Connection : public std::enable_shared_from_this<Connection>
	{
		// bool Reconnect();
	public:
		struct ConnectionInfo
		{
			ConnectionInfo();

			int			db_num;
			std::string host;
			int			port;
			std::string userid;
			std::string password;
			std::string database;
			std::string charset;
			int			timeout;
			bool		fail_query_log;
		};

		/*
		struct ReleaseFunctor
		{
			Connection* operator()(Connection* conn);
		};
		*/

		Connection();
		virtual ~Connection();

		bool Connect(const ConnectionInfo& connInfo);
		std::shared_ptr<ResultSetImpl> Execute(const std::string& query);
		//void AsyncExecute(const std::string& query, const std::function<void(const std::shared_ptr<ResultSetImpl>)>& callback);
		std::string RealEscapeString(const std::string& str);
		void Release();
	public :
		MYSQL			mysql_conn;
	private :
		ConnectionInfo	conn_info;
		Log::Logger*	fail_query_logger;
	};

	/*
	class ConnectionManager
	{
	public :
		typedef Pool<Connection, std::mutex, Policy::Initialze::do_nothing, Connection::ReleaseFunctor> CONN_POOL_T;

		bool Connect(int db_type, const Connection::ConnectionInfo& connInfo);
		std::shared_ptr<Connection> GetConnection(int db_type);
	private :
		Connection* ConnectionFactory(int db_type);
	private :
		CONN_POOL_T conn_pool;
		std::map<int, std::shared_ptr<CONN_POOL_T>> conn_pools;
		std::map<int, Connection::ConnectionInfo> conn_infos;;
	};
	*/
}

#endif /* CONNECTION_H_ */
