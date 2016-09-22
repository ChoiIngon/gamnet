#include "ColumnValue.h"

Gamnet::Database::ColumnValue::ColumnValue(const std::string& val) : value(val)
{
}

Gamnet::Database::ColumnValue::operator double() const
{
	return boost::lexical_cast<double>(value);
}

Gamnet::Database::ColumnValue::operator float() const 
{ 
	return boost::lexical_cast<float>(value); 
}

Gamnet::Database::ColumnValue::operator uint16_t() const 
{ 
	return boost::lexical_cast<uint16_t>(value); 
}

Gamnet::Database::ColumnValue::operator uint32_t()	const 
{ 
	return boost::lexical_cast<uint32_t>(value); 
}

Gamnet::Database::ColumnValue::operator uint64_t()	const 
{ 
	return boost::lexical_cast<uint64_t>(value); 
}

Gamnet::Database::ColumnValue::operator int16_t() const 
{
	return boost::lexical_cast<int16_t>(value); 
}

Gamnet::Database::ColumnValue::operator int32_t() const 
{
	return boost::lexical_cast<int32_t>(value); 
}

Gamnet::Database::ColumnValue::operator int64_t() const 
{ 
	return boost::lexical_cast<int64_t>(value); 
}

Gamnet::Database::ColumnValue::	operator bool() const 
{ 
	return boost::lexical_cast<bool>(value); 
}

Gamnet::Database::ColumnValue::operator const char*() const 
{ 
	return value.c_str(); 
}

Gamnet::Database::ColumnValue::operator std::string() const 
{ 
	return value; 
}