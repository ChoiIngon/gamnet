#ifndef _GAMNET_DATABASE_CONNECTIONPOOL_H_
#define _GAMNET_DATABASE_CONNECTIONPOOL_H_

#include "../Library/Pool.h"
#include "../Log/Log.h"

namespace Gamnet {	namespace Database {

template <class CONNECTION_T>
class ConnectionPool {
	typedef Pool<CONNECTION_T, std::mutex> CONNECTION_POOL_T;
	std::map<int, std::shared_ptr<CONNECTION_POOL_T>> mapConnectionPool_;
	std::map<int, typename CONNECTION_T::ConnectionInfo> mapConnectionInfo_;

	CONNECTION_T* ConnectionFactory(int db_type)
	{
		auto itr = mapConnectionInfo_.find(db_type);
		if (mapConnectionInfo_.end() == itr)
		{
			LOG(GAMNET_ERR, "can't find connection information for db(db_type:", db_type, ")");
			throw Exception(0, "can't find connection information for db(db_type:", db_type, ")");
		}

		CONNECTION_T* conn = new CONNECTION_T();
		typename CONNECTION_T::ConnectionInfo& connInfo = itr->second;
		if (false == conn->Connect(connInfo))
		{
			delete conn;
			return NULL;
		}
		return conn;
	}
public :
	bool Connect(int db_type, const typename CONNECTION_T::ConnectionInfo& connInfo)
	{
		if (false == mapConnectionInfo_.insert(std::make_pair(db_type, connInfo)).second)
		{
			LOG(GAMNET_ERR, "duplicate connection info(db_type:", db_type, ")");
			return false;
		}
		if (false == mapConnectionPool_.insert(std::make_pair(db_type, std::shared_ptr<CONNECTION_POOL_T>(new CONNECTION_POOL_T(64, std::bind(&ConnectionPool<CONNECTION_T>::ConnectionFactory, this, db_type))))).second)
		{
			LOG(GAMNET_ERR, "duplicate connection info(db_type:", db_type, ")");
			return false;
		}

		// connect test
		std::shared_ptr<CONNECTION_T> conn = mapConnectionPool_[db_type]->Create();
		if (NULL == conn)
		{
			LOG(GAMNET_ERR, "connection fail(db_type:", db_type, ")");
			return false;
		}
		return true;
	}

	std::shared_ptr<CONNECTION_T> GetConnection(int db_type)
	{
		auto itr = mapConnectionPool_.find(db_type);
		if (mapConnectionPool_.end() == itr)
		{
			LOG(ERR, ErrorCode::InvalidKeyError, "can't find database connection(db_type:", db_type, ")");
			return nullptr;
		}

		std::shared_ptr<CONNECTION_T> conn(itr->second->Create());
		if (nullptr == conn)
		{
			LOG(ERR, ErrorCode::CreateInstanceFailError, "create Connection object error(db_type:", db_type, ")");
			return nullptr;
		}
		return conn;
	}
};

} }
#endif
