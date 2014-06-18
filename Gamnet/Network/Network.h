/*
 * Network.h
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_NETWORK_H_
#define GAMNET_NETWORK_NETWORK_H_

#include "Session.h"
#include "Listener.h"

namespace Gamnet { namespace Network {
	template <class SESSION_T>
	void Listen(int port, int max_session, int keep_alive)
	{
		Singleton<Listener<SESSION_T>>::GetInstance().Init(port, max_session, keep_alive);
		Log::Write(GAMNET_INF, "Gamnet listener start(port:", port, ", capacity:", max_session, ", keep alive time:", keep_alive, "sec)");
	}

	template <class SESSION_T, class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Listener<SESSION_T>>::GetInstance().RegisterHandler(msg_id, func, factory);
	}

	template <class SESSION_T, class MSG>
	bool SendMsg(std::shared_ptr<SESSION_T> session, const MSG& msg)
	{
		std::shared_ptr<Packet> packet = Packet::Create();
		if(NULL == packet)
		{
			Log::Write(GAMNET_ERR, "fail to create packet instance(session_key:", session->sessionKey_, ", msg_id:", MSG::MSG_ID, ")");
			return false;
		}
		if(false == packet->Write(msg))
		{
			Log::Write(GAMNET_ERR, "fail to serialize message(session_key:", session->sessionKey_, ", msg_id:", MSG::MSG_ID, ")");
			return false;
		}
		if(0 > session->Send(packet))
		{
			Log::Write(GAMNET_ERR, "fail to send message(session_key:", session->sessionKey_, ", msg_id:", MSG::MSG_ID, ")");
			return false;
		}
		return true;
	}
}}

#define GAMNET_BIND_NETWORK_HANDLER(session_type, message_type, class_type, func, policy) \
	static bool Network_##class_type##_##func = Gamnet::Network::RegisterHandler<session_type>( \
			message_type::MSG_ID, \
			&class_type::func, \
			std::shared_ptr<Gamnet::Network::policy<class_type>>(new Gamnet::Network::policy<class_type>()) \
	)

#endif /* NETWORK_H_ */
