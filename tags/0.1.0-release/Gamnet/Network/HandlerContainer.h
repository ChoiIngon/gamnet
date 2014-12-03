/*
 * HandlerContainer.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HANDLERCONTAINER_H_
#define GAMNET_NETWORK_HANDLERCONTAINER_H_

#include <map>
#include <stdint.h>
#include <memory>

namespace Gamnet { namespace Network {

struct IHandler;

struct HandlerContainer
{
private :
	std::map<uint32_t, std::shared_ptr<IHandler>> mapHandler_;
public :
	HandlerContainer();
	virtual ~HandlerContainer();

	void Init();
	std::shared_ptr<IHandler> Find(uint32_t msg_id);
	void Register(uint32_t msg_id, std::shared_ptr<IHandler> handler);
};

}} /* namespace Gamnet */
#endif /* HANDLERCONTAINER_H_ */
