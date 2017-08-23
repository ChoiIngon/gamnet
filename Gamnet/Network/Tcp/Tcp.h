/*
 * Network.h
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_TCP_H_
#define GAMNET_NETWORK_TCP_H_

#include "LinkManager.h"
#include "../../Library/Json/json.h"

namespace Gamnet { namespace Network { namespace Tcp {
	template <class SESSION_T>
	void Listen(int port, int max_session, int keep_alive)
	{
		Singleton<LinkManager<SESSION_T>>::GetInstance().Listen(port, max_session, keep_alive);
		LOG(GAMNET_INF, "Gamnet listener start(port:", port, ", capacity:", max_session, ", keep alive time:", keep_alive, " sec)");
	}

	template <class SESSION_T, class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().RegisterHandler(msg_id, func, factory);
	}

	template <class SESSION_T, class MSG>
	bool SendMsg(const std::shared_ptr<SESSION_T>& session, const MSG& msg)
	{
		std::shared_ptr<Packet> packet = Packet::Create();
		if(NULL == packet)
		{
			LOG(GAMNET_ERR, "fail to create packet instance(session_key:", session->session_key, ", msg_id:", MSG::MSG_ID, ")");
			return false;
		}
		if(false == packet->Write<MSG>(session->msg_seq, msg))
		{
			LOG(GAMNET_ERR, "fail to serialize message(session_key:", session->session_key, ", msg_id:", MSG::MSG_ID, ")");
			return false;
		}
		session->AsyncSend(packet);
		return true;
	}

	boost::asio::ip::address GetLocalAddress();

	template <class SESSION_T>
	std::shared_ptr<Link> FindLink(uint32_t link_key)
	{
		return LinkManager<SESSION_T >>::GetInstance().Find(link_key);
	}

	template <class SESSION_T>
	std::shared_ptr<SESSION_T> FindSession(const std::string& session_key)
	{
		return Singleton<LinkManager<SESSION_T>>::GetInstance().session_manager.Find(session_key);
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
		_snprintf_s(date_time, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#else
		localtime_r(&logtime_, &when);
		snprintf(date_time, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#endif
		root["date_time"] = date_time;

		Json::Value session;
		session["running_count"] = (int)Singleton<LinkManager<SESSION_T>>::GetInstance().session_manager.Size();
		session["idle_count"] = (int)Singleton<LinkManager<SESSION_T>>::GetInstance().session_pool.Available();
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
}}}

#define GAMNET_BIND_TCP_HANDLER(session_type, message_type, class_type, func, policy) \
	static bool Network_##message_type##_##class_type##_##func = Gamnet::Network::Tcp::RegisterHandler<session_type>( \
			message_type::MSG_ID, \
			&class_type::func, \
			new Gamnet::Network::policy<class_type>() \
	)

#endif /* NETWORK_H_ */
