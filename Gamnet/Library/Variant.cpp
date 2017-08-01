#include "Variant.h"

Gamnet::Variant::Variant(const std::string& val) : value(val)
{
}

Gamnet::Variant::operator double() const
{
	return boost::lexical_cast<double>(value);
}

Gamnet::Variant::operator float() const 
{ 
	return boost::lexical_cast<float>(value); 
}

Gamnet::Variant::operator uint16_t() const 
{ 
	return boost::lexical_cast<uint16_t>(value); 
}

Gamnet::Variant::operator uint32_t()	const 
{ 
	return boost::lexical_cast<uint32_t>(value); 
}

Gamnet::Variant::operator uint64_t()	const 
{ 
	return boost::lexical_cast<uint64_t>(value); 
}

Gamnet::Variant::operator int16_t() const 
{
	return boost::lexical_cast<int16_t>(value); 
}

Gamnet::Variant::operator int32_t() const 
{
	return boost::lexical_cast<int32_t>(value); 
}

Gamnet::Variant::operator int64_t() const 
{ 
	return boost::lexical_cast<int64_t>(value); 
}

Gamnet::Variant::	operator bool() const 
{ 
	return boost::lexical_cast<bool>(value); 
}

Gamnet::Variant::operator const std::string () const
{
	return value;
}

