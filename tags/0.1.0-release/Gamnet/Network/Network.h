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
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().RegisterHandler(msg_id, func, factory);
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
	boost::asio::ip::address GetLocalAddress();
	template <class SESSION_T>
	std::shared_ptr<SESSION_T> FindSession(uint64_t session_key)
	{
		return std::static_pointer_cast<SESSION_T>(Singleton<Listener<SESSION_T>>::GetInstance().sessionManager_.FindSession(session_key));
	}

	template <class SESSION_T>
	std::string ServerState(const std::string& name)
	{
		uint32_t runningSessionCount = Singleton<Listener<SESSION_T>>::GetInstance().sessionManager_.Size();
		uint32_t idleSessionCount = Singleton<Listener<SESSION_T>>::GetInstance().sessionPool_.Available();
		time_t logtime_;
		struct tm when;
		time(&logtime_);
		localtime_r( &logtime_, &when );

		char timebuf[22] = {0};
		snprintf (timebuf, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year+1900, when.tm_mon+1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);

		std::string ret;
		ret += "{";
		ret += Format("\"date_time\":\"", timebuf,"\", ");
		ret += Format("\"name\":\"", name, "\", ");
		ret += Format("\"session\":{\"running_count\":", runningSessionCount, ", \"idle_count\":", idleSessionCount, "}");
		ret += "}";
		return ret;
	}
}}

#define GAMNET_BIND_NETWORK_HANDLER(session_type, message_type, class_type, func, policy) \
	static bool Network_##class_type##_##func = Gamnet::Network::RegisterHandler<session_type>( \
			message_type::MSG_ID, \
			&class_type::func, \
			new Gamnet::Network::policy<class_type>() \
	)

#endif /* NETWORK_H_ */
