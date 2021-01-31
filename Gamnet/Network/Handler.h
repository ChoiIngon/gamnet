/*
 * Handler.h
 *
 *  Created on: Jun 5, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_HANDLER_H_
#define GAMNET_NETWORK_HANDLER_H_

#include <memory>
#include "Router/AsyncResponse.h"
#include "../Library/Buffer.h"
#include "../Library/Time/Timer.h"

namespace Gamnet { namespace Network {

struct IHandler : public std::enable_shared_from_this<IHandler>
{
	IHandler() {}
	virtual ~IHandler() {}

	template<class MsgType, class RecvFuncType, class ExceptFuncType = typename Router::AsyncResponse<MsgType>::OnExceptionFuncType>
	std::shared_ptr<Router::IAsyncResponse> BindResponse(int timeout, RecvFuncType onReceive, ExceptFuncType onException = nullptr)
	{
		std::shared_ptr<Router::AsyncResponse<MsgType>> response = Router::AsyncResponse<MsgType>::Create();
		response->handler = shared_from_this();
		response->timeout = timeout * 1000;
		response->on_receive = static_cast<typename Router::AsyncResponse<MsgType>::OnReceiveFuncType>(onReceive);
		response->on_exception = static_cast<typename Router::AsyncResponse<MsgType>::OnExceptionFuncType>(onException);

		return response;
	}
};


}} /* namespace Gamnet */
#endif /* HANDLER_H_ */
