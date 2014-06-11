/*
 * Session.h
 *
 *  Created on: 2012. 10. 8.
 *      Author: jjaehuny
 */

#ifndef GAMNET_NETWORK_SESSION_H_
#define GAMNET_NETWORK_SESSION_H_

#include <map>
#include <mutex>
#include <list>
#include <boost/asio.hpp>
#include "Packet.h"
#include "HandlerContainer.h"

namespace Gamnet { namespace Network {

class IListener;
class Session : public std::enable_shared_from_this<Session>
{
public :
	struct Init
	{
		template <class T>
		T* operator() (T* session)
		{
			session->OnCreate();
			return session;
		}
	};

	boost::asio::ip::tcp::socket socket_;
	boost::asio::strand strand_;
	uint64_t sessionKey_;
	IListener* listener_;
	std::shared_ptr<Packet> readBuffer_;
	time_t lastHeartBeatTime_;
	HandlerContainer handlerContainer_;

	Session();
	virtual ~Session();

	void OnCreate();
	virtual void OnError(int reason);

	void _read_start();
	int	Send(std::shared_ptr<Packet> buffer);
private :
	int Send(const char* buf, int len);
};
}}
#endif /* NETWORK_SESSION_H_ */
