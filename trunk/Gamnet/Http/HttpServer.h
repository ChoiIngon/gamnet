/*
 * HttpServer.h
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "Listener.h"
namespace Gamnet { namespace Http {
	void Listen(int port, int max_session, int keep_alive);
}}

#endif /* HTTPSERVER_H_ */
