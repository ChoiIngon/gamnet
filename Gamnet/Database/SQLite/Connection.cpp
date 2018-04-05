#include "Connection.h"
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database { namespace SQLite {
	Connection::Connection() : conn_(nullptr)
	{
	}

	Connection::~Connection()
	{
		if(nullptr != conn_)
		{
			sqlite3_close(conn_);
		}
	}

	bool Connection::Connect(const ConnectionInfo&  connInfo)
	{
		connInfo_ = connInfo;
		if(SQLITE_OK != sqlite3_open(connInfo_.db_.c_str(), &conn_))
		{
			LOG(GAMNET_ERR, "[SQLite] open db fail(file_name:", connInfo_.db_, ")");
			return false;
		}
		return true;
	}

	std::shared_ptr<ResultSetImpl> Connection::Execute(const std::string& query)
	{
		std::shared_ptr<ResultSetImpl> impl = std::make_shared<ResultSetImpl>();
		char* errorMessage = nullptr;
		if(SQLITE_OK != sqlite3_exec(conn_, query.c_str(), ExecCallback, impl.get(), &errorMessage))
		{
			LOG(GAMNET_ERR, errorMessage, "(query:", query, ")");
			sqlite3_free(errorMessage);
			throw GAMNET_EXCEPTION(ErrorCode::UndefinedError);
		}
		return impl;
	}

	int Connection::ExecCallback(void *arg, int columnCount, char **rowDatas, char **columnName)
	{
		ResultSetImpl* resultSetImpl = (ResultSetImpl*)arg;
		if(0 == resultSetImpl->vecRows_.size())
		{
			for(int i=0; i<columnCount; i++)
			{
				if(resultSetImpl->mapColumnName_.end() == resultSetImpl->mapColumnName_.find(columnName[i]))
				{
					resultSetImpl->mapColumnName_[columnName[i]] = i;
				}
			}
		}
		std::vector<std::string> vecRow(columnCount);
		for (int i = 0; i<columnCount; i++)
		{
			if(nullptr != rowDatas[i])
			{
				vecRow[i] = rowDatas[i];
			}
		}
		resultSetImpl->vecRows_.push_back(vecRow);
		resultSetImpl->rowCount_ = resultSetImpl->vecRows_.size();
		return 0;
	}
	
}}}