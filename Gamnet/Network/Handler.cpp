/*
 * Handler.cpp
 *
 *  Created on: Jun 11, 2014
 *      Author: kukuta
 */

#include <atomic>
#include "Handler.h"
namespace Gamnet { namespace Network {

static std::atomic_uint handler_id(1);

IHandler::IHandler()
{
}

IHandler::~IHandler()
{
}

}}



