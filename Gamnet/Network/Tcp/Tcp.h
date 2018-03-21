/*
 * Network.h
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#ifndef GAMNET_NETWORK_TCP_H_
#define GAMNET_NETWORK_TCP_H_

#include "LinkManager.h"
#include "CastGroup.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace Gamnet { namespace Network { namespace Tcp {
	template <class SESSION_T>
	void Listen(int port, int max_session, int keep_alive)
	{
		Singleton<LinkManager<SESSION_T>>::GetInstance().Listen(port, max_session, keep_alive);
		LOG(GAMNET_INF, "Gamnet::Tcp listener start(port:", port, ", capacity:", max_session, ", keep alive time:", keep_alive, " sec)");
	}

	template <class SESSION_T>
	void ReadXml(const char* xml_path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

		int port = ptree_.get<int>("server.session.<xmlattr>.port");
		int max_count = ptree_.get<int>("server.session.<xmlattr>.max_count");
		int keep_alive = ptree_.get<int>("server.session.<xmlattr>.keep_alive");
		Listen<SESSION_T>(port, max_count, keep_alive);
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
	std::shared_ptr<SESSION_T> FindSession(uint32_t session_key)
	{
		return Singleton<LinkManager<SESSION_T>>::GetInstance().FindSession(session_key);
	}

	template <class SESSION_T>
	void DestroySession(uint32_t session_key)
	{
		Singleton<LinkManager<SESSION_T>>::GetInstance().DestroySession(session_key);
	}

	template <class SESSION_T>
	Json::Value  ServerState()
	{
		return Singleton<LinkManager<SESSION_T>>::GetInstance().State();
	}
}}}

#define GAMNET_BIND_TCP_HANDLER(session_type, message_type, class_type, func, policy) \
	static bool Network_##message_type##_##func = Gamnet::Network::Tcp::RegisterHandler<session_type>( \
			message_type::MSG_ID, \
			&class_type::func, \
			new Gamnet::Network::policy<class_type>() \
	)

#endif /* NETWORK_H_ */
