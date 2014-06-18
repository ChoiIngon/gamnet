/*
 * Gamnet.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_GAMNET_H_
#define GAMNET_GAMNET_H_

#include "Library/HttpClient.h"
#include "Library/Singleton.h"
#include "Log/Log.h"
#include "Network/Network.h"
#include "Router/Router.h"
#include "Test/Test.h"
#include "Database/Database.h"

namespace Gamnet {
void Run(int thread_count = 0);
}
#endif /* GAMNET_H_ */
