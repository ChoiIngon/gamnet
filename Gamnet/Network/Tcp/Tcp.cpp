/*
 * Network.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: kukuta
 */

#include <sys/types.h>
#include "Tcp.h"
#ifdef _WIN32
#else
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

namespace Gamnet { namespace Network { namespace Tcp {

	
boost::asio::ip::address GetLocalAddress()
{
#ifdef _WIN32
	boost::asio::ip::tcp::resolver resolver_(Singleton<boost::asio::io_service>::GetInstance());
	boost::asio::ip::tcp::resolver::query query_(boost::asio::ip::host_name(), ""); 
	boost::asio::ip::tcp::resolver::iterator itr = resolver_.resolve(query_);
	boost::asio::ip::address addr;
	while (itr != boost::asio::ip::tcp::resolver::iterator())
	{
		addr = (itr++)->endpoint().address();
		if (addr.is_v4())
		{
			break;
		}
	}
	return addr;
#else
	ifaddrs* ifAddrStruct=NULL;
	getifaddrs(&ifAddrStruct);
	uint32_t ip = 0;
	for (ifaddrs* ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa ->ifa_addr->sa_family==AF_INET) // check it is IP4
		{
			// is a valid IP4 Address
			ip = ((sockaddr_in *)ifa->ifa_addr)->sin_addr.s_addr;
		}
	}
	if (ifAddrStruct!=NULL)
	{
		freeifaddrs(ifAddrStruct);
	}
	return boost::asio::ip::address_v4(ntohl(ip));
#endif
}

}}}


