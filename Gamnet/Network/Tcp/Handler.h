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
		response->on_receive = static_cast<AsyncResponse<MsgType>::OnReceiveFuncType>(onReceive);
		response->on_exception = static_cast<AsyncResponse<MsgType>::OnExceptionFuncType>(onException);
		response->timer.AutoReset(false);
		response->timer.SetTimer(timeout, [response]() {
			if (nullptr != response->on_exception)
			{
				response->on_exception(response->handler, GAMNET_EXCEPTION(ErrorCode::ResponseTimeoutError));
			}
		});
		return response;
	}
};

}}} /* namespace Gamnet */
#endif /* HANDLER_H_ */
