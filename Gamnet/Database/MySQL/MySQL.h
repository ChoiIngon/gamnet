#ifndef GAMNET_DATABASE_MYSQL_MYSQL_H_
#define GAMNET_DATABASE_MYSQL_MYSQL_H_

#include "Connection.h"
#include "Transaction.h"
#include <functional>

namespace Gamnet { namespace Database { namespace MySQL {
	void ReadXml(const char* xml_path);
	bool Connect(int db_type, const std::string& host, int port, const std::string& id, const std::string& passwd, const std::string& db, bool fail_query_log = false);
	ResultSet Execute(int db_type, const std::string& query);
	//void _AsyncExecute(int db_type, const std::string& query, const std::function<void(ResultSet&)>& callback);
	template <class... ARGS>
	ResultSet Execute(int db_type, ARGS... args)
	{
		return Execute(db_type, Format(args...));
	}

	std::string RealEscapeString(int db_type, const std::string& str);
	
	/*
	template <class T1>
	void AsyncExecute(int db_type, T1&& t1, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1), callback);
	}
	template <class T1, class T2>
	void AsyncExecute(int db_type, T1&& t1, T2&& t2, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1, t2), callback);
	}
	template <class T1, class T2, class T3>
	void AsyncExecute(int db_type, T1&& t1, T2&& t2, T3&& t3, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1, t2, t3), callback);
	}
	template <class T1, class T2, class T3, class T4>
	void AsyncExecute(int db_type, T1&& t1, T2&& t2, T3&& t3, T4&& t4, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1, t2, t3, t4), callback);
	}
	template <class T1, class T2, class T3, class T4, class T5>
	void AsyncExecute(int db_type, T1&& t1, T2&& t2, T3&& t3, T4&& t4, T5&& t5, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1, t2, t3, t4, t5), callback);
	}
	template <class T1, class T2, class T3, class T4, class T5, class T6>
	void AsyncExecute(int db_type, T1&& t1, T2&& t2, T3&& t3, T4&& t4, T5&& t5, T6&& t6, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1, t2, t3, t4, t5, t6), callback);
	}
	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
	void AsyncExecute(int db_type, T1&& t1, T2&& t2, T3&& t3, T4&& t4, T5&& t5, T6&& t6, T7&& t7, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1, t2, t3, t4, t5, t6, t7), callback);
	}
	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
	void AsyncExecute(int db_type, T1&& t1, T2&& t2, T3&& t3, T4&& t4, T5&& t5, T6&& t6, T7&& t7, T8&& t8, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1, t2, t3, t4, t5, t6, t7, t8), callback);
	}
	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
	void AsyncExecute(int db_type, T1&& t1, T2&& t2, T3&& t3, T4&& t4, T5&& t5, T6&& t6, T7&& t7, T8&& t8, T9&& t9, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1, t2, t3, t4, t5, t6, t7, t8, t9), callback);
	}
	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
	void AsyncExecute(int db_type, T1&& t1, T2&& t2, T3&& t3, T4&& t4, T5&& t5, T6&& t6, T7&& t7, T8&& t8, T9&& t9, T10&& t10, const std::function<void(ResultSet&)>& callback)
	{
		_AsyncExecute(db_type, Format(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10), callback);
	}
	*/
}}}

#endif /* DATABASE_H_ */
