#include "SessionManager.h"
#include "RouterHandler.h"
#include "RouterCaster.h"
#include "../Tcp/Tcp.h"

import Gamnet.Singleton;

namespace Gamnet { namespace Network { namespace Router {
	SessionManager::SessionFactory::SessionFactory(SessionManager* manager) : session_manager(manager)
	{
	}
	
	Session* SessionManager::SessionFactory::operator() ()
	{
		Session* session = new Session();
		session->session_manager = session_manager;
		return session;
	}

	SessionManager::SessionManager()
		: session_pool(65535, SessionFactory(this))
		, port(0)
	{
		acceptor.accept_handler = std::bind(&SessionManager::OnAcceptHandler, this, std::placeholders::_1);
#ifdef _DEBUG
		connector.name = "Router::SessionManager";
#endif
		connector.connect_handler = std::bind(&SessionManager::OnConnectHandler, this, std::placeholders::_1);
	}

	SessionManager::~SessionManager() 
	{
	}
	
	void SessionManager::Listen(const Address& localRouterAddress, int port, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose, int accept_queue_size)
	{
		on_connect = onConnect;
		on_close = onClose;

		//local_address = Address(ROUTER_CAST_TYPE::UNI_CAST, serviceName, Network::Tcp::GetLocalAddress().to_v4().to_ulong());
		local_address = localRouterAddress;
		if (0 == local_address.id)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidAddressError, "unique router id is not set");
		}
		this->port = port;

		acceptor.Listen(port, 1024);
		
		std::shared_ptr<LocalSession> session = std::make_shared<LocalSession>();
		session->session_manager = this;
		session->remote_endpoint.address(Network::Tcp::GetLocalAddress());
		session->remote_endpoint.port(port);
		session->Init();
		Singleton<RouterCaster>::GetInstance().RegisterAddress( local_address, session );
	}

	void SessionManager::Connect(const std::string& host, int port, int timeout)
	{
		if(this->port == port)
		{
			if("127.0.0.1" == host || "localhost" == host || Tcp::GetLocalAddress().to_v4().to_string() == host)
			{
				return;
			}
		}
		connector.AsyncConnect(host, port, timeout);
	}

	/*
	void SessionManager::Add(const std::shared_ptr<Network::Session>& session)
	{
		Tcp::CastGroup::LockGuard lo(heartbeat_group);
		heartbeat_group->Insert(std::static_pointer_cast<Session>(session));
	}
	
	void SessionManager::Remove(const std::shared_ptr<Network::Session>& session)
	{
		Tcp::CastGroup::LockGuard lo(heartbeat_group);
		heartbeat_group->Remove(std::static_pointer_cast<Session>(session));
	}
	*/
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

		LOG(INF, "[Gamnet::Network::Router] accept a connection..(remote_endpoint:", socket->remote_endpoint().address().to_v4().to_string(), ":", socket->remote_endpoint().port(), ")");
		session->socket = socket;
		session->AsyncRead();
	}

	void SessionManager::OnConnectHandler(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
	{
		std::shared_ptr<Session> session = session_pool.Create();
		if (nullptr == session)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionError, "[Gamnet::Network::Route] can not create session instance(availble:", session_pool.Available(), ")");
		}

		LOG(INF, "[Gamnet::Network::Router] connect success..(remote_endpoint:", socket->remote_endpoint().address().to_v4().to_string(), ":", socket->remote_endpoint().port(), ")");
		session->socket = socket;
		session->AsyncRead();
		
		LOG(INF, "[Gamnet::Network::Router] "
			"[localhost:", port, " -> ", socket->remote_endpoint().address().to_v4().to_string(), ":", socket->remote_endpoint().port(), "] ",
			"SEND MsgRouter_Connect_Req(router_address:", local_address.ToString(), ")"
		);

		MsgRouter_Connect_Req req;
		req.service_name = local_address.service_name;
		req.id = local_address.id;

		std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
		packet->Write(req);
		session->Network::Session::AsyncSend(packet);
	}
}}}
