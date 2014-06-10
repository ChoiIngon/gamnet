/*
 * Listener.cpp
 *
 *  Created on: Jun 4, 2014
 *      Author: kukuta
 */
#include "Listener.h"
#include "Session.h"

namespace Gamnet { namespace Network {
std::atomic_ullong IListener::uniqueSessionKey_;
static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>();

}}



