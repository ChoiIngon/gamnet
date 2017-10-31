/*
 * Network.h
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_TCP_H_
#define GAMNET_NETWORK_TCP_H_

#include "LinkManager.h"

namespace Gamnet { namespace Network { namespace Tcp {
	template <class SESSION_T>
	void Listen(int port, int max_session, int keep_alive)
	{
		Singleton<LinkManager<SESSION_T>>::GetInstance().Listen(port, max_session, keep_alive);
		LOG(GAMNET_INF, "Gamnet::Tcp listener start(port:", port, ", capacity:", max_session, ", keep alive time:", keep_alive, " sec)");
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
		return Singleton<LinkManager<SESSION_T >>::GetInstance().Find(link_key);
	}

	template <class SESSION_T>
	std::shared_ptr<SESSION_T> FindSession(uint32_t session_key)
	{
		return std::static_pointer_cast<SESSION_T>(Singleton<LinkManager<SESSION_T>>::GetInstance().session_manager.Find(session_key));
	}

	template <class SESSION_T>
	Json::Value  ServerState()
	{
		return Singleton<LinkManager<SESSION_T>>::GetInstance().State();
	}
}}}

#define GAMNET_BIND_TCP_HANDLER(session_type, message_type, class_type, func, policy) \
	static bool Network_##message_type##_##class_type##_##func = Gamnet::Network::Tcp::RegisterHandler<session_type>( \
			message_type::MSG_ID, \
			&class_type::func, \
			new Gamnet::Network::policy<class_type>() \
	)

#endif /* NETWORK_H_ */
