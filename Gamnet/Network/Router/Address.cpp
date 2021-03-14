#include "Address.h"

#include <sstream>

namespace Gamnet { namespace Network { namespace Router {

Address::Address()
	: service_name("")
	, cast_type(CAST_TYPE::UNI_CAST)
	, id(0)
	, msg_seq(0)
{
}

Address::Address(const std::string& service_name, CAST_TYPE cast_type, uint32_t id)
	: service_name(service_name)
	, cast_type(cast_type)
	, id(id)
	, msg_seq(0)
{
}

std::string Address::ToString() const
{
	std::stringstream ss;
	ss << "{\"service_name\":\"" << service_name << "\", \"cast_type\":" << (int)cast_type << ", \"id\":" << id << "}";
	return ss.str();
}

bool operator == (const Address& lhs, const Address& rhs)
{
	if (lhs.service_name == rhs.service_name && (int)lhs.cast_type == (int)rhs.cast_type && lhs.id == rhs.id)
	{
		return true;
	}
	return false;
}

bool operator != (const Address& lhs, const Address& rhs)
{
	return !(lhs == rhs);
}

bool operator <  (const Address& lhs, const Address& rhs)
{
	if (lhs.id < rhs.id)
	{
		return true;
	}
	else if (lhs.id == rhs.id)
	{
		if (lhs.service_name < rhs.service_name)
		{
			return true;
		}
		else if (lhs.service_name == rhs.service_name)
		{
			if ((int)lhs.cast_type < (int)rhs.cast_type)
			{
				return true;
			}
		}
	}
	return false;
}

bool operator > (const Address& lhs, const Address& rhs)
{
	if (lhs == rhs)
	{
		return false;
	}

	if (lhs < rhs)
	{
		return false;
	}

	return true;
}

bool operator <= (const Address& lhs, const Address& rhs)
{
	if (lhs == rhs || lhs < rhs)
	{
		return true;
	}
	return false;
}

bool operator >= (const Address& lhs, const Address& rhs)
{
	if (lhs == rhs || lhs > rhs)
	{
		return true;
	}
	return false;
}

}}}