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
}}



