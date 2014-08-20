/*
 * ResultSet.cpp
 *
 *  Created on: Jun 14, 2014
 *      Author: kukuta
 */
#include "ResultSet.h"

namespace Gamnet { namespace Database {

ResultSet::iterator::iterator()
	: hasNext_(false), resultSet_(NULL)
{
}

ResultSet::iterator::iterator(const iterator& itr)
{
	*this = itr;
}

ResultSet::iterator& ResultSet::iterator::operator = (const iterator& itr)
{
	hasNext_ = itr.hasNext_;
	resultSet_ = itr.resultSet_;
	return *this;
}

ResultSet::iterator& ResultSet::iterator::operator ++ (int)
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

std::shared_ptr<sql::ResultSet> ResultSet::iterator::operator -> ()
{
	return resultSet_;
}

bool ResultSet::iterator::operator != (const ResultSet::iterator& itr) const
{
	if(this->hasNext_ != itr.hasNext_ || resultSet_ != itr.resultSet_)
	{
		return true;
	}
	return false;
}

bool ResultSet::iterator::operator == (const ResultSet::iterator& itr) const
{
	if(this->hasNext_ == itr.hasNext_ && resultSet_ == itr.resultSet_)
	{
		return true;
	}
	return false;
}

ResultSet::ResultSet() : affectedRowCount_(0), errno_(0)
{
}

ResultSet::~ResultSet()
{
}

int ResultSet::GetSQLError() const
{
	return errno_;
};

int ResultSet::GetAffectedRow() const
{
	return affectedRowCount_;
}

int ResultSet::GetRowCount()
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

ResultSet::iterator ResultSet::begin()
{
	iterator itr;
	if(NULL != resultSet_)
	{
		itr.resultSet_ = resultSet_;
		itr.hasNext_ = resultSet_->first();
	}
	return itr;
}

ResultSet::iterator ResultSet::end() const
{
	iterator itr;
	if(NULL != resultSet_)
	{
		itr.resultSet_ = resultSet_;
		itr.hasNext_ = false;
	}
	return itr;
}

ResultSet::iterator ResultSet::operator [] (int index)
{
	iterator itr;
	if(NULL != resultSet_ && resultSet_->absolute(index+1))
	{
		itr.resultSet_ = resultSet_;
		itr.hasNext_ = false;
	}
	return itr;
}
}}



