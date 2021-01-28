/*
 * Handler.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HANDLER_H_
#define GAMNET_NETWORK_HANDLER_H_

#include <memory>
#include "../Library/Buffer.h"
#include "../Library/Time/Timer.h"

namespace Gamnet { namespace Network {

struct IHandler : public std::enable_shared_from_this<IHandler>
{
	IHandler() {}
	virtual ~IHandler() {}
};


}} /* namespace Gamnet */
#endif /* HANDLER_H_ */
