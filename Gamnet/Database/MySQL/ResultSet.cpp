#include "ResultSet.h"
#include "Connection.h"
#include <boost/lexical_cast.hpp>
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database { namespace MySQL {

ResultSetImpl::ResultSetImpl(const std::shared_ptr<Connection> conn) :
		conn_(conn),
		res_(nullptr),
		affectedRowCount_(0),
		lastInsertID_(0)
{
}

ResultSetImpl::~ResultSetImpl()
{
	if (nullptr != res_)
	{
		mysql_free_result(res_);
		res_ = nullptr;
	}

	// trash remain result
	if (mysql_more_results(&(conn_->conn_)))
	{
		while (0 == mysql_next_result(&conn_->conn_))
		{
			MYSQL_RES* res = mysql_store_result(&conn_->conn_);
			if (nullptr != res)
			{
				mysql_free_result(res);
			}
		}
	}
}

bool ResultSetImpl::Execute(const std::string& query)
{
	assert(nullptr != conn_);
	if(0 != mysql_real_query(&(conn_->conn_), query.c_str(), query.length()))
	{
		LOG(GAMNET_ERR, mysql_error(&(conn_->conn_)), "(query:", query, ")");
		throw Exception(mysql_errno(&(conn_->conn_)), mysql_error(&(conn_->conn_)), "(query:", query, ")");
	}

	if(false == StoreResult())
	{
		LOG(GAMNET_ERR, mysql_error(&(conn_->conn_)), "(query:", query, ")");
		throw Exception(mysql_errno(&(conn_->conn_)), mysql_error(&(conn_->conn_)), "(query:", query, ")");
	}
	return true;
}

std::string ResultSetImpl::RealEscapeString(const std::string& str)
{
	std::vector<char> buff(str.length() * 2 + 1);
	unsigned long length = mysql_real_escape_string(&(conn_->conn_), &buff[0], str.c_str(), str.length());

	return std::string(&buff[0], length);
}

bool ResultSetImpl::StoreResult()
{
	affectedRowCount_ = 0;
	lastInsertID_ = 0;
	mapColumnName_.clear();

	if (nullptr != res_)
	{
		mysql_free_result(res_);
		res_ = nullptr;
	}

	unsigned int num_fields = mysql_field_count(&conn_->conn_);
	if (0 < num_fields)
	{
		res_ = mysql_store_result(&conn_->conn_);
		if (nullptr == res_)
		{
			LOG(ERR, mysql_error(&conn_->conn_));
			throw Exception(mysql_errno(&conn_->conn_), mysql_error(&conn_->conn_));
		}
		MYSQL_FIELD *field = nullptr;
		for (unsigned int i = 0; (field = mysql_fetch_field(res_)); i++)
		{
			mapColumnName_[field->name] = i;
		}
	}
	else
	{
		affectedRowCount_ = (unsigned int)mysql_affected_rows(&(conn_->conn_));
		lastInsertID_ = static_cast<unsigned int>(mysql_insert_id(&(conn_->conn_)));
	}

	return true;
}

bool ResultSetImpl::NextResult()
{
	if(nullptr == &conn_->conn_)
	{
		return false;
	}
	if(false == mysql_more_results(&conn_->conn_))
	{
		return false;
	}

	if(0 != mysql_next_result(&conn_->conn_))
	{
		throw Exception(mysql_errno(&conn_->conn_), mysql_error(&conn_->conn_));
	}
	
	return StoreResult();
}

ResultSet::ResultSet() : impl_(NULL)
{
}

ResultSet::~ResultSet()
{
}

unsigned int ResultSet::GetAffectedRow() const
{
	if (nullptr == impl_)
	{
		return 0;
	}
	return impl_->affectedRowCount_;
}

unsigned int ResultSet::GetRowCount()
{
	if (nullptr == impl_)
	{
		return 0;
	}
	if (nullptr == impl_->res_)
	{
		return 0;
	}
	return (unsigned int)mysql_num_rows(impl_->res_);
}

unsigned int ResultSet::GetLastInsertID() const
{
	return impl_->lastInsertID_;
}

bool ResultSet::NextResult()
{
	return impl_->NextResult();
}

Json::Value ResultSet::ToJson()
{
	Json::Value root;
	if (nullptr != impl_ && nullptr != impl_->res_)
	{
		unsigned int num_fields = impl_->mapColumnName_.size();
		std::vector<std::string> columnNames(num_fields);
		for(const auto& itr : impl_->mapColumnName_)
		{
			columnNames[itr.second] = itr.first;
		}

		mysql_data_seek(impl_->res_, 0);

		MYSQL_ROW row = nullptr;
		while(nullptr != (row = mysql_fetch_row(impl_->res_)))
		{
			Json::Value value;
			for(unsigned int i=0; i<num_fields; i++)
			{
				value[columnNames[i]] = row[i];
			}		
			root.append(value);
		}
	}
	return root;
}

ResultSet::iterator ResultSet::begin()
{
	return iterator(impl_);
}

ResultSet::iterator ResultSet::end() const
{
	return iterator();
}

ResultSet::iterator::iterator() : impl_(nullptr), row_(nullptr)
{
}

ResultSet::iterator::iterator(const std::shared_ptr<ResultSetImpl>& impl) : impl_(impl)
{
	if (nullptr != impl_)
	{
		mysql_data_seek(impl_->res_, 0);
		row_ = mysql_fetch_row(impl_->res_);
		if (nullptr != row_ && nullptr == *row_)
		{
			row_ = nullptr;
		}
	}
}

ResultSet::iterator& ResultSet::iterator::operator ++ (int)
{
	if (nullptr == impl_)
	{
		row_ = nullptr;
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
	if (nullptr == impl_ || nullptr == impl_->res_)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "invalid result set");
	}

	if (index >= mysql_num_rows(impl_->res_))
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidArrayRangeError, "out range row index(index:", index, ")");
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
		throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "Unknown column '", column_name, "' in 'field list'");
	}
	if (nullptr == row_)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "invalid data");
	}

	if(nullptr == row_[itr->second])
	{
		return "";
	}
	return row_[itr->second];
}

uint32_t ResultSet::iterator::getUInt(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<uint32_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
uint16_t ResultSet::iterator::getUInt16(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<uint16_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
uint32_t ResultSet::iterator::getUInt32(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<uint32_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
uint64_t ResultSet::iterator::getUInt64(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<uint64_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
int32_t ResultSet::iterator::getInt(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<int32_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
int16_t ResultSet::iterator::getInt16(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<int16_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
int32_t ResultSet::iterator::getInt32(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<int32_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
int64_t ResultSet::iterator::getInt64(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<int64_t>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
bool ResultSet::iterator::getBool(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<bool>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
float ResultSet::iterator::getFloat(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<float>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}
double ResultSet::iterator::getDouble(const std::string& column_name)
{
	try
	{
		return boost::lexical_cast<double>(getString(column_name));
	}
	catch (const boost::bad_lexical_cast& /* e */)
	{
		throw GAMNET_EXCEPTION(ErrorCode::BadLexicalCastError, "column_name:", column_name);
	}
}

Variant ResultSet::iterator::operator [] (const std::string& column_name)
{
	Variant var(getString(column_name));
	return var;
}

} } }



