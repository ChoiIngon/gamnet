/*
 * Network.cpp
 *
 *  Created on: Jun 20, 2014
 *      Author: kukuta
 */

#include "Network.h"
namespace Gamnet { namespace Network {

boost::asio::ip::address GetLocalAddress()
{
	boost::asio::ip::tcp::resolver resolver_(Singleton<boost::asio::io_service>::GetInstance());
	boost::asio::ip::tcp::endpoint endpoint_ = *resolver_.resolve({boost::asio::ip::host_name(), ""});
	return endpoint_.address();
}

}}


