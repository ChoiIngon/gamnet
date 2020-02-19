#ifndef GAMNET_NETWORK_TCP_H_
#define GAMNET_NETWORK_TCP_H_

#include "LinkManager.h"
#include "CastGroup.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace Gamnet { namespace Network { namespace Tcp {

	template <class SESSION_T>
	void Listen(int port, int max_session, int keep_alive, int accept_queue_size)
	{
		Singleton<Dispatcher<SESSION_T>>::GetInstance().BindHandler(MSG_ID::MsgID_CliSvr_Connect_Req, "MsgID_CliSvr_Connect_Req", &SystemMessageHandler<SESSION_T>::Recv_Connect_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
		Singleton<Dispatcher<SESSION_T>>::GetInstance().BindHandler(MSG_ID::MsgID_CliSvr_Reconnect_Req, "MsgID_CliSvr_Reconnect_Req", &SystemMessageHandler<SESSION_T>::Recv_Reconnect_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
		Singleton<Dispatcher<SESSION_T>>::GetInstance().BindHandler(MSG_ID::MsgID_CliSvr_Close_Req, "MsgID_CliSvr_Close_Req", &SystemMessageHandler<SESSION_T>::Recv_Close_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
		Singleton<Dispatcher<SESSION_T>>::GetInstance().BindHandler(MSG_ID::MsgID_CliSvr_HeartBeat_Req, "MsgID_CliSvr_HeartBeat_Req", &SystemMessageHandler<SESSION_T>::Recv_HeartBeat_Req, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
		Singleton<Dispatcher<SESSION_T>>::GetInstance().BindHandler(MSG_ID::MsgID_CliSvr_ReliableAck_Ntf, "MsgID_CliSvr_ReliableAck_Ntf", &SystemMessageHandler<SESSION_T>::Recv_ReliableAck_Ntf, new HandlerStatic<SystemMessageHandler<SESSION_T>>());
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
		int accept_queue = ptree_.get<int>("server.tcp.<xmlattr>.accept_queue");
		Listen<SESSION_T>(port, max_count, keep_alive, accept_queue);
	}

	template <class SESSION_T, class FUNC, class FACTORY>
	bool BindHandler(unsigned int msg_id, const char* name, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().BindHandler(msg_id, name, func, factory);
	}

	template <class SESSION_T, class MSG>
	void SendMsg(const std::shared_ptr<SESSION_T>& session, const MSG& msg, bool reliable = false)
	{
		std::shared_ptr<Packet> packet = Packet::Create();
		if (nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create Packet instance(session_key:", session->session_key, ", msg_id:", MSG::MSG_ID, ")");
		}

		packet->msg_seq = ++session->send_seq;
		packet->reliable = reliable;
		if(false == packet->Write(msg))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(session_key:", session->session_key, ", msg_id:", MSG::MSG_ID, ")");
		}

		session->AsyncSend(packet);
	}

	const boost::asio::ip::address& GetLocalAddress();
	
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
	static bool Network_##message_type##_##func = Gamnet::Network::Tcp::BindHandler<session_type>( \
			message_type::MSG_ID, \
			#message_type, \
			&class_type::func, \
			new Gamnet::Network::policy<class_type>() \
	)

#endif /* NETWORK_H_ */
