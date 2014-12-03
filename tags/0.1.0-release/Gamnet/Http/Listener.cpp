/*
 * Listener.cpp
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#include "Listener.h"

namespace Gamnet {
namespace Http {

Listener::Listener() : acceptor_(Singleton<boost::asio::io_service>::GetInstance()), port_(0), sessionPool_()
{
}

Listener::~Listener() {
}

} /* namespace Http */
} /* namespace Gamnet */
