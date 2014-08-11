/*
 * Session.h
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#ifndef SESSION_H_
#define SESSION_H_

#include "../Network/Network.h"
#include "Response.h"

namespace Gamnet {
namespace Http {

class Session : public Network::Session {
public:
	struct Init
	{
		template <class T>
		T* operator() (T* session)
		{
			session->socket_.close();
			session->sessionKey_ = 0;
			session->listener_ = NULL;
			session->readBuffer_ = Network::Packet::Create();
			session->lastHeartBeatTime_ = ::time(NULL);
			session->handlerContainer_.Init();
			return session;
		}
	};
	Session();
	virtual ~Session();

	int Send(const Response& res);

	virtual void OnAccept()
	{
	}
	virtual void OnClose(int reason)
	{
	}

	virtual void AsyncRead();

	boost::asio::streambuf buf_;
};

} /* namespace Http */
} /* namespace Gamnet */
#endif /* SESSION_H_ */
