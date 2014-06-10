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

namespace Gamnet { namespace Database {
struct ResultSet
{
	struct iterator
	{
		bool hasNext_;
		std::shared_ptr<sql::ResultSet> resultSet_;
		iterator() : hasNext_(false)
		{
		}
		iterator(const iterator& itr)
		{
			*this = itr;
		}
		iterator& operator = (const iterator& itr)
		{
			*this = itr;
			return *this;
		}
		iterator& operator ++ (int)
		{
			if(NULL == resultSet_)
			{
				hasNext_ = false;
			}
			else
			{
				hasNext_ = resultSet_->next();
			}
			return *this;
		}
		std::shared_ptr<sql::ResultSet> operator -> ()
		{
			return resultSet_;
		}
		bool operator != (ResultSet::iterator itr)
		{
			if(this->hasNext_ != itr.hasNext_ || resultSet_ != itr.resultSet_)
			{
				return true;
			}
			return false;
		}

		bool operator == (ResultSet::iterator itr)
		{
			if(this->hasNext_ == itr.hasNext_ && resultSet_ == itr.resultSet_)
			{
				return true;
			}
			return false;
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
