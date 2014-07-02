/*
 * Network.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: kukuta
 */

#include "Network.h"
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace Gamnet { namespace Network {

boost::asio::ip::address GetLocalAddress()
{
	/*
	boost::asio::ip::tcp::resolver resolver_(Singleton<boost::asio::io_service>::GetInstance());
	boost::asio::ip::tcp::endpoint endpoint_ = *resolver_.resolve({boost::asio::ip::host_name(), ""});
	return endpoint_.address();
	*/
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
	return boost::asio::ip::address_v4(::ntohl(ip));
}

}}


