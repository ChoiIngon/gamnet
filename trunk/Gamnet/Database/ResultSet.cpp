/*
 * ResultSet.cpp
 *
 *  Created on: Jun 14, 2014
 *      Author: kukuta
 */
#include "ResultSet.h"
#include "Connection.h"
#include "../Library/Exception.h"
namespace Gamnet { namespace Database {

ResultSetImpl::ResultSetImpl() : res_(NULL), affectedRowCount_(0), conn_(NULL)
{
}

ResultSetImpl::~ResultSetImpl()
{
	if(NULL != res_)
	{
		mysql_free_result(res_) ;
		res_ = NULL;

		// trash remain result
		if (mysql_more_results(&(conn_->conn_)))
		{
			while (0 == mysql_next_result(&conn_->conn_))
			{
				MYSQL_RES* res = mysql_store_result(&conn_->conn_);
				if(NULL != res)
				{
					mysql_free_result(res);
				}
			}
		}
	}
}

ResultSet::iterator::iterator() : impl_(NULL), row_(NULL)
{
}

ResultSet::iterator::iterator(const std::shared_ptr<ResultSetImpl>& impl) : impl_(impl)
{
	if(NULL != impl_)
	{
		mysql_data_seek(impl_->res_, 0);
		row_ = mysql_fetch_row(impl_->res_);
	}
}

ResultSet::iterator& ResultSet::iterator::operator ++ (int)
{
	if(NULL == impl_)
	{
		row_ = NULL;
	}
	else
	{
		row_ = mysql_fetch_row(impl_->res_);
	}
	return *this;
}

const std::string ResultSet::iterator::getString(const std::string& column_name)
{
	auto itr = impl_->mapColumnName_.find(column_name);
	if(impl_->mapColumnName_.end() == itr)
	{
		throw Exception(0, "can't find column(name:", column_name, ")");
	}
	if(NULL == row_)
	{
		throw Exception(0, "invalid data");
	}

	return row_[itr->second];
}

ResultSet::iterator* ResultSet::iterator::operator -> ()
{
	return this;
}

bool ResultSet::iterator::operator != (const ResultSet::iterator& itr) const
{
	if(row_ != itr.row_)
	{
		return true;
	}

	return false;
}

bool ResultSet::iterator::operator == (const ResultSet::iterator& itr) const
{
	if(row_ != itr.row_)
	{
		return false;
	}

	return true;
}

ResultSet::ResultSet() : impl_(NULL)
{
}

ResultSet::~ResultSet()
{
}

unsigned int ResultSet::GetAffectedRow() const
{
	return impl_->affectedRowCount_;
}

unsigned int ResultSet::GetRowCount()
{
	if(NULL == impl_)
	{
		return 0;
	}
	if(NULL == impl_->res_)
	{
		return 0;
	}
	return mysql_num_rows(impl_->res_);
}

ResultSet::iterator ResultSet::begin()
{
	return iterator(impl_);
}

ResultSet::iterator ResultSet::end() const
{
	return iterator();
}

ResultSet::iterator ResultSet::operator [] (unsigned int index)
{
	iterator itr;
	if(NULL == impl_ || NULL == impl_->res_)
	{
		throw Exception(0, "invalid result set");
	}

	if(0 > index || index >= mysql_num_rows(impl_->res_))
	{
		throw Exception(0, "out range row index(index:", index, ")");
	}
	mysql_data_seek(impl_->res_, index);
	itr.impl_ = impl_;
	itr.row_ = mysql_fetch_row(impl_->res_);

	return itr;
}

}}



