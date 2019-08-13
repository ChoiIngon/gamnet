#include "Variant.h"

namespace Gamnet {
Variant::Variant()
{
}

Variant::Variant(const Variant& val) : value(val.value)
{
}

Variant::Variant(const std::string& val) : value(val)
{
}

Variant::Variant(const char* val) : value(val)
{
}

Variant::Variant(bool val) 
{
	value = boost::lexical_cast<std::string>(val);
}

Variant::Variant(double val)
{
	value = boost::lexical_cast<std::string>(val);
}

Variant::Variant(float val)
{
	value = boost::lexical_cast<std::string>(val);
}

Variant::Variant(uint16_t val)
{
	value = boost::lexical_cast<std::string>(val);
}

Variant::Variant(uint32_t val)
{
	value = boost::lexical_cast<std::string>(val);
}

Variant::Variant(uint64_t val)
{
	value = boost::lexical_cast<std::string>(val);
}

Variant::Variant(int16_t val)
{
	value = boost::lexical_cast<std::string>(val);
}

Variant::Variant(int32_t val)
{
	value = boost::lexical_cast<std::string>(val);
}

Variant::Variant(int64_t val)
{
	value = boost::lexical_cast<std::string>(val);
}

Variant::operator double() const
{
	return boost::lexical_cast<double>(value);
}

Variant::operator float() const 
{ 
	return boost::lexical_cast<float>(value); 
}

Variant::operator uint16_t() const 
{ 
	return boost::lexical_cast<uint16_t>(value); 
}

Variant::operator uint32_t()	const 
{ 
	return boost::lexical_cast<uint32_t>(value); 
}

Variant::operator uint64_t()	const 
{ 
	return boost::lexical_cast<uint64_t>(value); 
}

Variant::operator int16_t() const 
{
	return boost::lexical_cast<int16_t>(value); 
}

Variant::operator int32_t() const 
{
	return boost::lexical_cast<int32_t>(value); 
}

Variant::operator int64_t() const 
{ 
	return boost::lexical_cast<int64_t>(value); 
}

Variant::operator bool() const
{ 
	return boost::lexical_cast<bool>(value); 
}
Variant::operator std::string()
{
	return value;
}
Variant::operator std::string() const
{
	return value;
}
const Variant& Variant::operator = (const Variant& rhs)
{
	value = rhs.value;
	return *this;
}
const Variant& Variant::operator = (const std::string& rhs)
{
	value = rhs;
	return *this;
}
const Variant& Variant::operator = (bool rhs)
{
	value = boost::lexical_cast<std::string>(rhs);
	return *this;
}
const Variant& Variant::operator = (double rhs)
{
	value = boost::lexical_cast<std::string>(rhs);
	return *this;
}
const Variant& Variant::operator = (float rhs)
{
	value = boost::lexical_cast<std::string>(rhs);
	return *this;
}
const Variant& Variant::operator = (uint16_t rhs)
{
	value = boost::lexical_cast<std::string>(rhs);
	return *this;
}
const Variant& Variant::operator = (uint32_t rhs)
{
	value = boost::lexical_cast<std::string>(rhs);
	return *this;
}
const Variant& Variant::operator = (uint64_t rhs)
{
	value = boost::lexical_cast<std::string>(rhs);
	return *this;
}
const Variant& Variant::operator = (int16_t rhs)
{
	value = boost::lexical_cast<std::string>(rhs);
	return *this;
}
const Variant& Variant::operator = (int32_t rhs)
{
	value = boost::lexical_cast<std::string>(rhs);
	return *this;
}
const Variant& Variant::operator = (int64_t rhs)
{
	value = boost::lexical_cast<std::string>(rhs);
	return *this;
}

bool Variant::operator < (const Variant& rhs)
{
	return value < rhs.value;
}

bool Variant::operator == (const Variant& rhs)
{
	return value == rhs.value;
}

bool Variant::operator != (const Variant& rhs)
{
	return value != rhs.value;
}
}