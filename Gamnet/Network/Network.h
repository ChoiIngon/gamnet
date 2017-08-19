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
#include "../Json/json.h"

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
		Json::Value root;
		root["name"] = name;

		time_t logtime_;
		struct tm when;
		time(&logtime_);

		char date_time[22] = {0};
#ifdef _WIN32
		localtime_s(&when, &logtime_);
		_snprintf(date_time, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#else
		localtime_r(&logtime_, &when);
		snprintf(date_time, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#endif
		root["date_time"] = date_time;

		Json::Value session;
		session["running_count"] = (int)Singleton<Listener<SESSION_T>>::GetInstance().sessionManager_.Size();
		session["idle_count"] = (int)Singleton<Listener<SESSION_T>>::GetInstance().sessionPool_.Available();
		root["session"] = session;

#ifdef _DEBUG
		Json::Value message;
		for(auto itr : Singleton<Dispatcher<SESSION_T>>::GetInstance().mapHandlerCallStatistics_)
		{
			Json::Value statistics;
			statistics["msg_id"] = (int)itr.second->msg_id;
			statistics["begin_count"] = (int)itr.second->begin_count;
			statistics["finish_count"] = (int)itr.second->finish_count;
			statistics["elapsed_time"] = (int)itr.second->elapsed_time;
			if(0 == itr.second->elapsed_time || 0 == itr.second->finish_count)
			{
				statistics["average_time"] = 0;
			}
			else
			{
				statistics["average_time"] = (int)(itr.second->elapsed_time/itr.second->finish_count);
			}
			message.append(statistics);
		}
		root["message"] = message;
#endif
		Json::StyledWriter writer;
		return writer.write(root);
	}
}}

#define GAMNET_BIND_NETWORK_HANDLER(session_type, message_type, class_type, func, policy) \
	static bool Network_##message_type##_##class_type##_##func = Gamnet::Network::RegisterHandler<session_type>( \
			message_type::MSG_ID, \
			&class_type::func, \
			new Gamnet::Network::policy<class_type>() \
	)

#endif /* NETWORK_H_ */
