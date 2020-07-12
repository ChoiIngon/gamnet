#ifndef GAMNET_NETWORK_TCP_H_
#define GAMNET_NETWORK_TCP_H_

#include "CastGroup.h"
#include "Dispatcher.h"
#include "SessionManager.h"

namespace Gamnet { namespace Network { namespace Tcp {

	template <class SESSION_T>
	void Listen(int port, int max_session, int keep_alive, int accept_queue_size)
	{
		Singleton<Dispatcher<SESSION_T>>::GetInstance();
		Singleton<SessionManager<SESSION_T>>::GetInstance().Listen(port, max_session, keep_alive, accept_queue_size);
		LOG(GAMNET_INF, "Gamnet::Tcp listener start(port:", port, ", capacity:", max_session, ", keep alive time:", keep_alive, " sec)");
	}

	struct Config
	{
		int port;
		int max_count;
		int keep_alive;
		int accept_queue;
		int thread_count;

		void ReadXml(const std::string& path);
	};

	template <class SESSION_T>
	void ReadXml(const std::string& path)
	{
		Config config;
		config.ReadXml(path);
		Listen<SESSION_T>(config.port, config.max_count, config.keep_alive, config.accept_queue);
	}

	template <class SESSION_T, class FUNC, class FACTORY>
	bool BindHandler(unsigned int msg_id, const char* name, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().BindHandler(msg_id, name, func, factory);
	}

	template <class SESSION_T, class MSG>
	void SendMsg(const std::shared_ptr<SESSION_T>& session, const MSG& msg, bool reliable = true)
	{
		if(nullptr == session)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "null session");
		}

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
		return Singleton<SessionManager<SESSION_T>>::GetInstance().Find(session_key);
	}

	template <class SESSION_T>
	void DestroySession(uint32_t session_key)
	{
		Singleton<SessionManager<SESSION_T>>::GetInstance().OnDestroy(session_key);
	}

	template <class SESSION_T>
	Json::Value  ServerState()
	{
		return Singleton<SessionManager<SESSION_T>>::GetInstance().State();
	}
}}}

#define TOKEN_PASTE(x, y) x##y
#define TOKEN_PASTE2(x, y) TOKEN_PASTE(x, y)

#define GAMNET_BIND_TCP_HANDLER(session_type, message_type, class_type, func, policy) \
	static bool TOKEN_PASTE2(Network_##message_type##_##func, __LINE__) = Gamnet::Network::Tcp::BindHandler<session_type>( \
			message_type::MSG_ID, \
			#message_type, \
			&class_type::func, \
			new Gamnet::Network::policy<class_type>() \
	)

#endif /* NETWORK_H_ */
