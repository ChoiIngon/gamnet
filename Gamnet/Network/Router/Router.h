#ifndef GAMNET_NETWORK_ROUTER_H_
#define GAMNET_NETWORK_ROUTER_H_

#include "RouterCaster.h"
#include "Dispatcher.h"


namespace Gamnet { namespace Network { namespace Router 
{
	void ReadXml(const std::string& path, const std::function<void(const Address& addr)>& connectHandler = [](const Address&) {}, const std::function<void(const Address& addr)>& closeHandler = [](const Address&) {});

	const Address& GetRouterAddress();
	void Listen(const std::string& serviceName, int port, const std::function<void(const Address& addr)>& acceptHandler = [](const Address&){}, const std::function<void(const Address& addr)>& closeHandler = [](const Address&) {});
	void Connect(const std::string& host, int port, int timeout);

	template <class FUNC, class FACTORY>
	bool BindHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher>::GetInstance().BindHandler(msg_id, func, factory);
	}

	class SendResult
	{
		uint32_t msg_seq;
	public:
		SendResult(uint32_t msgSEQ);
		template<class MSG>
		void WaitResponse(const std::shared_ptr<Network::Tcp::Session>& session, std::function<void()> onTimeout)
		{
			if(nullptr == session->current_handler)
			{
				throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "current message handler is null");
			}
			session->handler_container.Register(msg_seq, session->current_handler);

			std::shared_ptr<Dispatcher::WaitResponse> waitResponse = std::make_shared<Dispatcher::WaitResponse>();
			waitResponse->expire_time = time(nullptr) + 60;
			waitResponse->on_timeout = onTimeout;
			waitResponse->session = session;
			Singleton<Dispatcher>::GetInstance().RegisterWaitResponse(MSG::MSG_ID, msg_seq, waitResponse);
		}
	};
	template <class MSG>
	SendResult SendMsg(const Address& addr, const MSG& msg)
	{
		std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
		if(nullptr == packet)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", MSG::MSG_ID, ")");
		}

		packet->msg_seq = 0;
		packet->reliable = false;

		if(false == packet->Write(msg))
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", MSG::MSG_ID, ")");
		}

		Singleton<RouterCaster>::GetInstance().SendMsg(addr, packet);
		return SendResult(addr.msg_seq);
	}
}}}

#define GAMNET_BIND_ROUTER_HANDLER(message_type, class_type, func, policy) \
	static bool Router_##message_type##_##func = Gamnet::Network::Router::BindHandler( \
		message_type::MSG_ID, \
		&class_type::func, \
		new Gamnet::Network::policy<class_type>() \
	)

#endif /* ROUTER_H_ */
