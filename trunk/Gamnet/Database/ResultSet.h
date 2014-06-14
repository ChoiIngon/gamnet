/*
 * ResultSet.h
 *
 *  Created on: Jul 25, 2013
 *      Author: kukuta
 */

#ifndef GAMNET_DATABASE_RESULTSET_H_
#define GAMNET_DATABASE_RESULTSET_H_

#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <memory>
#include <boost/lexical_cast.hpp>

namespace Gamnet { namespace Database {
struct ResultSet
{
	struct iterator
	{
		bool hasNext_;
		std::shared_ptr<sql::ResultSet> resultSet_;
		iterator();
		iterator(const iterator& itr);
		iterator& operator = (const iterator& itr);
		iterator& operator ++ (int);
		std::shared_ptr<sql::ResultSet> operator -> ();

		bool operator != (ResultSet::iterator itr);
		bool operator == (ResultSet::iterator itr);
		const char* operator [] (const std::string& column_name)
		{
			return resultSet_->getString(column_name).c_str();
		}
		template <class T>
		const T GetValue(const std::string& column_name)
		{
			return boost::lexical_cast<T>(resultSet_->getString(column_name));
		}
	};

	int affectedRowCount_;
	int errno_;
	std::shared_ptr<sql::ResultSet> resultSet_;

	ResultSet() : affectedRowCount_(0), errno_(0) {}
	virtual ~ResultSet() {}


	int GetSQLError() { return errno_; };
	int GetAffectedRow() { return affectedRowCount_; }
	int GetRowCount()
	{
		int rowCount = 0;
		try
		{
			resultSet_->last();
			rowCount = resultSet_->getRow();
			resultSet_->beforeFirst();
		}
		catch(const sql::SQLException& e)
		{
			return 0;
		}
		return rowCount;
	}
	iterator begin()
	{
		iterator itr;
		if(NULL != resultSet_)
		{
			itr.resultSet_ = resultSet_;
			itr.hasNext_ = resultSet_->first();
		}
		return itr;
	}
	iterator end() const
	{
		iterator itr;
		if(NULL != resultSet_)
		{
			itr.resultSet_ = resultSet_;
			itr.hasNext_ = false;
		}
		return itr;
	}
	iterator operator [] (int index)
	{
		iterator itr;
		if(NULL != resultSet_ && resultSet_->absolute(index+1))
		{
			itr.resultSet_ = resultSet_;
			itr.hasNext_ = false;
		}
		return itr;
	}
};

}}
#endif /* RESULTSET_H_ */
