/*
 * TestSession.cpp
 *
 *  Created on: Jun 7, 2014
 *      Author: kukuta
 */

#include "Session.h"

#include "LinkManager.h"
namespace Gamnet { namespace Test {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Session::Session() : test_seq(-1) {
}

Session::~Session() {
}
}}/* namespace Gamnet */
