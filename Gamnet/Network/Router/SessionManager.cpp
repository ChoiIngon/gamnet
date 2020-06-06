#include "SessionManager.h"
#include "RouterHandler.h"
#include "../Tcp/Tcp.h"

namespace Gamnet { namespace Network { namespace Router {
	SessionManager::SessionManager() : heartbeat_timer(Time::Timer::Create()), heartbeat_group(Tcp::CastGroup::Create())
	{
		acceptor.accept_handler = std::bind(&SessionManager::OnAcceptHandler, this, std::placeholders::_1);
		connector.connect_handler = std::bind(&SessionManager::OnConnectHandler, this, std::placeholders::_1);
	}

	SessionManager::~SessionManager() 
	{
	}
	
	void SessionManager::Listen(const std::string& serviceName, int port, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose, int accept_queue_size)
	{
		on_connect = onConnect;
		on_close = onClose;

		local_address = Address(ROUTER_CAST_TYPE::UNI_CAST, serviceName, Network::Tcp::GetLocalAddress().to_v4().to_ulong());
		if (0 == local_address.id)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidAddressError, "unique router id is not set");
		}
		this->port = port;

		heartbeat_timer->AutoReset(true);
		heartbeat_timer->SetTimer(60000, [this]() {
			MsgRouter_HeartBeat_Ntf ntf;
			Tcp::CastGroup::LockGuard lockedCastGroup(heartbeat_group);
			LOG(GAMNET_INF, "[Router] send heartbeat message(connected server count:", lockedCastGroup->Size(), ")");
			lockedCastGroup->SendMsg(ntf);
		});

		acceptor.Listen(port, 1024);
	}

	void SessionManager::Connect(const std::string& host, int port, int timeout)
	{
		connector.AsyncConnect(host, port, timeout);
	}

	void SessionManager::Add(const std::shared_ptr<Network::Session>& session)
	{
		Tcp::CastGroup::LockGuard lo(heartbeat_group);
		heartbeat_group->Insert(std::static_pointer_cast<Session>(session));
	}
	
	void SessionManager::OnReceive(const std::shared_ptr<Network::Session>& session, const std::shared_ptr<Buffer>& buffer)
	{
		Singleton<Tcp::Dispatcher<Session>>::GetInstance().OnReceive(std::static_pointer_cast<Session>(session), std::static_pointer_cast<Tcp::Packet>(buffer));
	}

	Json::Value SessionManager::State()
	{
		Json::Value root;
		root["name"] = "Gamnet::Network::Router";
		root["address"] = local_address.ToString();
		Json::Value session;
		session["max_count"] = (unsigned int)session_pool.Capacity();
		session["idle_count"] = (unsigned int)session_pool.Available();
		root["session"] = session;
		return root;
	}

	void SessionManager::OnAcceptHandler(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
	{
		std::shared_ptr<Session> session = session_pool.Create();
		if (nullptr == session)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionError, "[Gamnet::Network::Route] can not create session instance(availble:", session_pool.Available(), ")");
		}

		session->socket = socket;
		session->OnCreate();
		session->OnAccept();
		Add(session);
	}

	void SessionManager::OnConnectHandler(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
	{
		std::shared_ptr<Session> session = session_pool.Create();
		if (nullptr == session)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionError, "[Gamnet::Network::Route] can not create session instance(availble:", session_pool.Available(), ")");
		}

		session->socket = socket;
		session->OnCreate();

		Add(session);

		//LOG(INF, "[", session->session_key, "] connect success..(remote ip:", session->GetRemoteAddress().to_string(), ")");
		//LOG(INF, "[", session->session_key, "] localhost->", session->GetRemoteAddress().to_string(), " SEND MsgRouter_SetAddress_Req(router_address:", Singleton<LinkManager>::GetInstance().local_address.ToString(), ")");
		MsgRouter_SetAddress_Req req;
		req.router_address = local_address;
		req.host = Network::Tcp::GetLocalAddress().to_string();
		req.port = port;
		Network::Tcp::SendMsg(session, req, false);
	}
}}}
