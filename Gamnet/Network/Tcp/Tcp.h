#ifndef GAMNET_NETWORK_TCP_H_
#define GAMNET_NETWORK_TCP_H_

#include "LinkManager.h"
#include "CastGroup.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace Gamnet { namespace Network { namespace Tcp {

	template <class SESSION_T>
	void Listen(int port, int max_session, int keep_alive, int accept_queue_size = 7)
	{
		Singleton<LinkManager<SESSION_T>>::GetInstance().Listen(port, max_session, keep_alive, accept_queue_size);
		LOG(GAMNET_INF, "Gamnet::Tcp listener start(port:", port, ", capacity:", max_session, ", keep alive time:", keep_alive, " sec)");
	}

	template <class SESSION_T>
	void ReadXml(const char* xml_path)
	{
		boost::property_tree::ptree ptree_;
		boost::property_tree::xml_parser::read_xml(xml_path, ptree_);

		int port = ptree_.get<int>("server.tcp.<xmlattr>.port");
		int max_count = ptree_.get<int>("server.tcp.<xmlattr>.max_count");
		int keep_alive = ptree_.get<int>("server.tcp.<xmlattr>.keep_alive");
		Listen<SESSION_T>(port, max_count, keep_alive);
	}

	template <class SESSION_T, class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, const char* name, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().RegisterHandler(msg_id, name, func, factory);
	}

	template <class SESSION_T, class MSG>
	bool SendMsg(const std::shared_ptr<SESSION_T>& session, const MSG& msg, bool reliable = false)
	{
		std::shared_ptr<Packet> packet = Packet::Create();
		if (nullptr == packet)
		{
			LOG(GAMNET_ERR, "fail to create packet instance(session_key:", session->session_key, ", msg_id:", MSG::MSG_ID, ")");
			return false;
		}

		packet->msg_seq = ++session->send_seq;
		packet->reliable = reliable;
		if(false == packet->Write(msg))
		{
			LOG(GAMNET_ERR, "fail to serialize message(session_key:", session->session_key, ", msg_id:", MSG::MSG_ID, ")");
			return false;
		}

		return session->AsyncSend(packet);
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
			#message_type, \
			&class_type::func, \
			new Gamnet::Network::policy<class_type>() \
	)

#endif /* NETWORK_H_ */
