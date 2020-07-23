/*
 * Handler.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HANDLER_H_
#define GAMNET_NETWORK_HANDLER_H_

#include <memory>
#include "Coroutine.h"
namespace Gamnet { namespace Network {

class Coroutine;
struct IHandler : public std::enable_shared_from_this<IHandler>
{
	Coroutine* coroutine;

	IHandler() : coroutine(nullptr) {}
	virtual ~IHandler() {}
/*
	template <class MSG>
	void BindMessage(std::shared_ptr<Session> session)
	{
		session->handler_container.Register(MSG::MSG_ID, shared_from_this());
	}
*/

	void resume()
	{
		coroutine->resume();
	}
	void yield()
	{
		coroutine->yield();
	}
};


}} /* namespace Gamnet */
#endif /* HANDLER_H_ */
