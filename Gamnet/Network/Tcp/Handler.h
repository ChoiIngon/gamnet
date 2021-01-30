/*
 * Handler.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_TCP_HANDLER_H_
#define GAMNET_NETWORK_TCP_HANDLER_H_

#include <memory>
#include "Packet.h"
#include "AsyncResponse.h"
#include "../Handler.h"
#include "../../Library/Buffer.h"
#include "../../Library/Time/Timer.h"

namespace Gamnet { namespace Network { namespace Tcp {

struct Handler : public IHandler
{
	Handler() {}
	virtual ~Handler() {}

	template<class MsgType, class RecvFuncType, class ExceptFuncType = typename AsyncResponse<MsgType>::OnExceptionFuncType>
	std::shared_ptr<IAsyncResponse> BindResponse(int timeout, RecvFuncType onReceive, ExceptFuncType onException = nullptr)
	{
		std::shared_ptr<AsyncResponse<MsgType>> response = AsyncResponse<MsgType>::Create();
		response->handler = shared_from_this();
		response->timeout = timeout * 1000;
		response->on_receive = static_cast<typename AsyncResponse<MsgType>::OnReceiveFuncType>(onReceive);
		response->on_exception = static_cast<typename AsyncResponse<MsgType>::OnExceptionFuncType>(onException);
		
		return response;
	}
};

}}} /* namespace Gamnet */
#endif /* HANDLER_H_ */
