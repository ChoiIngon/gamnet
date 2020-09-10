#include "ResultSet.h"
#include "Connection.h"
#include <boost/lexical_cast.hpp>
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database { namespace MySQL {

ResultSetImpl::ResultSetImpl(const std::shared_ptr<Connection> conn) :
	mysql_res(nullptr),
	affected_rows(0),
	last_insert_id(0),
	error_code(0),
	conn(conn)
{
}

ResultSetImpl::~ResultSetImpl()
{
	if (nullptr != mysql_res)
	{
		mysql_free_result(mysql_res);
		mysql_res = nullptr;
	}
	conn->Release();
}

bool ResultSetImpl::StoreResult()
{
	affected_rows = 0;
	last_insert_id = 0;
	columns.clear();

	if (nullptr != mysql_res)
	{
		mysql_free_result(mysql_res);
		mysql_res = nullptr;
	}

	unsigned int num_fields = mysql_field_count(&conn->mysql_conn);
	if (0 < num_fields)
	{
		mysql_res = mysql_store_result(&conn->mysql_conn);
		if (nullptr == mysql_res)
		{
			LOG(ERR, mysql_error(&conn->mysql_conn));
			throw Exception(mysql_errno(&conn->mysql_conn), mysql_error(&conn->mysql_conn));
		}
		MYSQL_FIELD *field = nullptr;
		for (unsigned int i = 0; (field = mysql_fetch_field(mysql_res)); i++)
		{
			columns[field->name] = i;
		}
	}
	else
	{
		affected_rows = mysql_affected_rows(&conn->mysql_conn);
		last_insert_id = mysql_insert_id(&conn->mysql_conn);
	}

	return true;
}

bool ResultSetImpl::NextResult()
{
	if(nullptr == &conn->mysql_conn)
	{
		return false;
	}
	if(false == mysql_more_results(&conn->mysql_conn))
	{
		return false;
	}

	if(0 != mysql_next_result(&conn->mysql_conn))
	{
		throw Exception(mysql_errno(&conn->mysql_conn), mysql_error(&conn->mysql_conn));
	}
	
	return StoreResult();
}

ResultSet::ResultSet(const std::shared_ptr<ResultSetImpl>& impl)
	: impl_(impl)
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
	return impl_->affected_rows;
}

unsigned int ResultSet::GetRowCount()
{
	if (nullptr == impl_)
	{
		return 0;
	}
	if (nullptr == impl_->mysql_res)
	{
		return 0;
	}
	return (unsigned int)mysql_num_rows(impl_->mysql_res);
}

unsigned int ResultSet::GetLastInsertID() const
{
	return impl_->last_insert_id;
}

unsigned int ResultSet::GetLastError() const
{
	return impl_->error_code;
}

bool ResultSet::NextResult()
{
	return impl_->NextResult();
}

Json::Value ResultSet::ToJson()
{
	Json::Value root;
	if (nullptr != impl_ && nullptr != impl_->mysql_res)
	{
		unsigned int num_fields = impl_->columns.size();
		std::vector<std::string> columnNames(num_fields);
		for(const auto& itr : impl_->columns)
		{
			columnNames[itr.second] = itr.first;
		}

		mysql_data_seek(impl_->mysql_res, 0);

		MYSQL_ROW row = nullptr;
		while(nullptr != (row = mysql_fetch_row(impl_->mysql_res)))
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

ResultSet& ResultSet::operator=(const std::shared_ptr<ResultSetImpl>& impl)
{
	impl_ = impl;
	return *this;
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
		mysql_data_seek(impl_->mysql_res, 0);
		row_ = mysql_fetch_row(impl_->mysql_res);
		if (nullptr != row_ && nullptr == *row_)
		{
			row_ = nullptr;
		}
	}
}

ResultSet::iterator& ResultSet::iterator::operator * ()
{
	return *this;
}

ResultSet::iterator& ResultSet::iterator::operator ++ ()
{
	if (nullptr == impl_)
	{
		row_ = nullptr;
	}
	else
	{
		row_ = mysql_fetch_row(impl_->mysql_res);
	}
	return *this;
}

ResultSet::iterator& ResultSet::iterator::operator ++ (int)
{
	if (nullptr == impl_)
	{
		row_ = nullptr;
	}
	else
	{
		row_ = mysql_fetch_row(impl_->mysql_res);
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
	if (nullptr == impl_ || nullptr == impl_->mysql_res)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "invalid result set");
	}

	if (index >= mysql_num_rows(impl_->mysql_res))
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidArrayRangeError, "out range row index(index:", index, ")");
	}

	mysql_data_seek(impl_->mysql_res, index);
	itr.impl_ = impl_;
	itr.row_ = mysql_fetch_row(impl_->mysql_res);

	return itr;
}

const std::string ResultSet::iterator::getString(const std::string& column_name) const
{
	auto itr = impl_->columns.find(column_name);
	if (impl_->columns.end() == itr)
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

uint32_t ResultSet::iterator::getUInt(const std::string& column_name) const
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
uint16_t ResultSet::iterator::getUInt16(const std::string& column_name) const
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
uint32_t ResultSet::iterator::getUInt32(const std::string& column_name) const
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
uint64_t ResultSet::iterator::getUInt64(const std::string& column_name) const
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
int32_t ResultSet::iterator::getInt(const std::string& column_name) const
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
int16_t ResultSet::iterator::getInt16(const std::string& column_name) const
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
int32_t ResultSet::iterator::getInt32(const std::string& column_name) const
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
int64_t ResultSet::iterator::getInt64(const std::string& column_name) const
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
bool ResultSet::iterator::getBool(const std::string& column_name) const
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
float ResultSet::iterator::getFloat(const std::string& column_name) const
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
double ResultSet::iterator::getDouble(const std::string& column_name) const
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

Variant ResultSet::iterator::operator [] (const std::string& column_name) const
{
	Variant var(getString(column_name));
	return var;
}

} } }



