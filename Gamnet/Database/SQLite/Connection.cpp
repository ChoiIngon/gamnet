#include "Connection.h"
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

#define SQLITE_ENABLE_UNLOCK_NOTIFY 1

namespace Gamnet { namespace Database { namespace SQLite {
	UnlockNotification::UnlockNotification() : fired(false)
	{
	}

	int WaitForUnlockNotify(Connection* conn)
	{
		conn->un_.fired = false;

		int errorCode = sqlite3_unlock_notify(conn->conn_, UnlockNotifyCallback, (void*)&conn->un_);
		if (SQLITE_OK == errorCode)
		{
			std::unique_lock<std::mutex> lo(conn->un_.lock);
			if (false == conn->un_.fired)
			{
				auto now = std::chrono::system_clock::now();
				if(std::cv_status::timeout == conn->un_.cond_var.wait_until(lo, now + std::chrono::seconds(5)))
				{
					return SQLITE_BUSY;
				}
			}
		}
		return errorCode;
	}

	void UnlockNotifyCallback(void** argv, int argc)
	{
		for (int i = 0; i<argc; i++)
		{
			UnlockNotification* p = (UnlockNotification*)argv[i];
			std::lock_guard<std::mutex> lo(p->lock);
			p->fired = true;
			p->cond_var.notify_one();
		}
	}

	int ExecCallback(void *arg, int columnCount, char **rowDatas, char **columnName)
	{
		ResultSetImpl* resultSetImpl = (ResultSetImpl*)arg;
		if (0 == resultSetImpl->vecRows_.size())
		{
			for (int i = 0; i<columnCount; i++)
			{
				if (resultSetImpl->mapColumnName_.end() == resultSetImpl->mapColumnName_.find(columnName[i]))
				{
					resultSetImpl->mapColumnName_[columnName[i]] = i;
				}
			}
		}

		std::vector<std::string> vecRow(columnCount);
		for (int i = 0; i<columnCount; i++)
		{
			if (nullptr != rowDatas[i])
			{
				vecRow[i] = rowDatas[i];
			}
		}
		resultSetImpl->vecRows_.push_back(vecRow);
		resultSetImpl->rowCount_ = resultSetImpl->vecRows_.size();
		return 0;
	}

	Connection::ConnectionInfo::ConnectionInfo() : db_(""), timeout_(100)
	{
	}

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

	bool Connection::Connect(const ConnectionInfo& connInfo)
	{
		sqlite3_enable_shared_cache(1);
		
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

		int errorCode = sqlite3_exec(conn_, query.c_str(), ExecCallback, impl.get(), &errorMessage);

		while(SQLITE_LOCKED == errorCode)
		{
			errorCode = WaitForUnlockNotify(this);
			if(SQLITE_OK != errorCode)
			{
				break;
			}

			if(nullptr != errorMessage)
			{
				sqlite3_free(errorMessage);
				errorMessage = nullptr;
			}
			errorCode = sqlite3_exec(conn_, query.c_str(), ExecCallback, impl.get(), &errorMessage);
		}

		if(SQLITE_OK != errorCode)
		{
			LOG(GAMNET_ERR, "error_code:", errorCode, ", error_message:", errorMessage, ", query:", query);
			if (nullptr != errorMessage)
			{
				sqlite3_free(errorMessage);
			}

			throw GAMNET_EXCEPTION((int)ErrorCode::UndefinedError);
		}
		
		return impl;
	}	
}}}