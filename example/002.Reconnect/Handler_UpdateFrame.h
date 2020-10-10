#ifndef HANDLER_UPDATE_FRAME_H_
#define HANDLER_UPDATE_FRAME_H_

#include "UserSession.h"

class Handler_UpdateFrame : public Gamnet::Network::IHandler
{
public:
	Handler_UpdateFrame();
	virtual ~Handler_UpdateFrame();

	void Recv_Req(const std::shared_ptr<UserSession>& session, const MsgCliSvr_UpdateFrame_Req& req);
};

#endif /* HANDLER_LOGIN_H_ */
