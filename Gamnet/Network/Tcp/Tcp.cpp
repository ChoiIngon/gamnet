#include "Tcp.h"

#include <boost/exception/diagnostic_information.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <sys/types.h>
#ifdef _WIN32
#else
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

namespace Gamnet { namespace Network { namespace Tcp {

	static boost::asio::ip::address localAddress = GetLocalAddress();
	
const boost::asio::ip::address& GetLocalAddress()
{
	if(false == localAddress.is_unspecified())
	{
		return localAddress;
	}
#ifdef _WIN32
	boost::asio::ip::tcp::resolver resolver_(Singleton<boost::asio::io_context>::GetInstance());
	boost::asio::ip::tcp::resolver::query query_(boost::asio::ip::host_name(), ""); 
	boost::asio::ip::tcp::resolver::iterator itr = resolver_.resolve(query_);
	
	while (itr != boost::asio::ip::tcp::resolver::iterator())
	{
		localAddress = (itr++)->endpoint().address();
		if (localAddress.is_v4())
		{
			break;
		}
	}
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
	localAddress = boost::asio::ip::address_v4(ntohl(ip));
#endif
	return localAddress;
}

Config::Config() 
	: port(0)
	, max_count(0)
	, keep_alive(0)
	, accept_queue(0)
	, thread_count(0)
{
}

void Config::ReadXml(const std::string& path)
{
	boost::property_tree::ptree ptree_;
	try {
		boost::property_tree::xml_parser::read_xml(path, ptree_);
	}
	catch (const boost::property_tree::xml_parser_error& e)
	{
		throw Exception(ErrorCode::FileNotFound, e.what());
	}

	auto optional = ptree_.get_child_optional("server.tcp");
	if (false == (bool)optional)
	{
		return;
	}

	try {
		port = ptree_.get<int>("server.tcp.<xmlattr>.port");
		max_count = ptree_.get<int>("server.tcp.<xmlattr>.max_count");
		keep_alive = ptree_.get<int>("server.tcp.<xmlattr>.keep_alive");
		accept_queue = ptree_.get<int>("server.tcp.<xmlattr>.accept_queue");
		thread_count = ptree_.get<int>("server.tcp.<xmlattr>.thread_count");
	}
	catch (const boost::property_tree::ptree_bad_path& e)
	{
		throw GAMNET_EXCEPTION(ErrorCode::SystemInitializeError, e.what());
	}
}
}}}


