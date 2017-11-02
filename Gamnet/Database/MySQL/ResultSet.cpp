#include "ResultSet.h"
#include "Connection.h"
#include "../../Library/Exception.h"
#include <boost/lexical_cast.hpp>

namespace Gamnet { namespace Database { namespace MySQL {

ResultSetImpl::ResultSetImpl() :
		conn_(NULL),
		res_(NULL),
		affectedRowCount_(0),
		lastInsertID_(0)
{
}

ResultSetImpl::~ResultSetImpl()
{
	if (NULL != res_)
	{
		mysql_free_result(res_);
		res_ = NULL;

		// trash remain result
		if (mysql_more_results(&(conn_->conn_)))
		{
			while (0 == mysql_next_result(&conn_->conn_))
			{
				MYSQL_RES* res = mysql_store_result(&conn_->conn_);
				if (NULL != res)
				{
					mysql_free_result(res);
				}
			}
		}
	}
}

ResultSet::ResultSet() : impl_(NULL)
{
}

ResultSet::~ResultSet()
{
}

unsigned int ResultSet::GetAffectedRow() const
{
	if (NULL == impl_)
	{
		return 0;
	}
	return impl_->affectedRowCount_;
}

unsigned int ResultSet::GetRowCount()
{
	if (NULL == impl_)
	{
		return 0;
	}
	if (NULL == impl_->res_)
	{
		return 0;
	}
	return mysql_num_rows(impl_->res_);
}

unsigned int ResultSet::GetLastInsertID() const
{
	return impl_->lastInsertID_;
}

ResultSet::iterator ResultSet::begin()
{
	return iterator(impl_);
}

ResultSet::iterator ResultSet::end() const
{
	return iterator();
}

ResultSet::iterator::iterator() : impl_(NULL), row_(NULL)
{
}

ResultSet::iterator::iterator(const std::shared_ptr<ResultSetImpl>& impl) : impl_(impl)
{
	if (NULL != impl_)
	{
		mysql_data_seek(impl_->res_, 0);
		row_ = mysql_fetch_row(impl_->res_);
		if (NULL != row_ && NULL == *row_)
		{
			row_ = NULL;
		}
	}
}

ResultSet::iterator& ResultSet::iterator::operator ++ (int)
{
	if (NULL == impl_)
	{
		row_ = NULL;
	}
	else
	{
		row_ = mysql_fetch_row(impl_->res_);
	}
	return *this;
}

ResultSet::iterator* ResultSet::iterator::operator -> ()
{
	return this;
}

bool ResultSet::iterator::operator != (const ResultSet::iterator& itr) const
{
	if (row_ != itr.row_)
	{
		return true;
	}

	return false;
}

bool ResultSet::iterator::operator == (const ResultSet::iterator& itr) const
{
	if (row_ != itr.row_)
	{
		return false;
	}

	return true;
}

ResultSet::iterator ResultSet::operator [] (unsigned int index)
{
	iterator itr;
	if (NULL == impl_ || NULL == impl_->res_)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "invalid result set");
	}

	if (index >= mysql_num_rows(impl_->res_))
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::InvalidArrayRangeError), "out range row index(index:", index, ")");
	}
	mysql_data_seek(impl_->res_, index);
	itr.impl_ = impl_;
	itr.row_ = mysql_fetch_row(impl_->res_);

	return itr;
}

const std::string ResultSet::iterator::getString(const std::string& column_name)
{
	auto itr = impl_->mapColumnName_.find(column_name);
	if (impl_->mapColumnName_.end() == itr)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::InvalidKeyError), "Unknown column '", column_name, "' in 'field list'");
	}
	if (NULL == row_)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "invalid data");
	}

	return row_[itr->second];
}

uint32_t ResultSet::iterator::getUInt(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<uint32_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
uint16_t ResultSet::iterator::getUInt16(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<uint16_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
uint32_t ResultSet::iterator::getUInt32(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<uint32_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
uint64_t ResultSet::iterator::getUInt64(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<uint64_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
int32_t ResultSet::iterator::getInt(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<int32_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
int16_t ResultSet::iterator::getInt16(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<int16_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
int32_t ResultSet::iterator::getInt32(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<int32_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
int64_t ResultSet::iterator::getInt64(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<int64_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
bool ResultSet::iterator::getBool(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<bool>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
float ResultSet::iterator::getFloat(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<float>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}
double ResultSet::iterator::getDouble(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<double>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& e)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::BadLexicalCastError), "column_name:", column_name);
	}
}

Variant ResultSet::iterator::operator [] (const std::string& column_name)
{
	Variant var(getString(column_name));
	return var;
}

} } }



