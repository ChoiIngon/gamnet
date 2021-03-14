#ifndef GAMNET_NETWORK_ROUTER_ADDRESS_H_
#define GAMNET_NETWORK_ROUTER_ADDRESS_H_

#include <string>

namespace Gamnet { namespace Network { namespace Router {

class Address
{
public :
	enum class CAST_TYPE
	{
		UNI_CAST,
		MULTI_CAST,
		ANY_CAST,
		MAX
	};

	Address();
	Address(const std::string& service_name, CAST_TYPE cast_type, uint32_t id = 0);

	std::string ToString() const;

	std::string	service_name;
	CAST_TYPE	cast_type;
	uint32_t	id;
	uint32_t	msg_seq;
};

bool operator == (const Address& lhs, const Address& rhs);
bool operator != (const Address& lhs, const Address& rhs);
bool operator <  (const Address& lhs, const Address& rhs);
bool operator >  (const Address& lhs, const Address& rhs);
bool operator <= (const Address& lhs, const Address& rhs);
bool operator >= (const Address& lhs, const Address& rhs);

}}}
#endif