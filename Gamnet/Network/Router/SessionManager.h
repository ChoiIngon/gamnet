#ifndef GAMNET_NETWORK_ROUTER_SESSION_MANAGER_H_
#define GAMNET_NETWORK_ROUTER_SESSION_MANAGER_H_

#include "Session.h"
#include "../../Library/Time/Time.h"
#include "../../Library/Pool.h"
#include "../SessionManager.h"
#include "../Tcp/CastGroup.h"
#include "../Tcp/Connector.h"
#include "../Tcp/Acceptor.h"

#include <list>
#include <set>

namespace Gamnet { namespace Network { namespace Router {
	class SessionManager : public Network::SessionManager
	{
	public :
		struct SessionFactory
		{
			SessionManager* const session_manager;
			SessionFactory(SessionManager* manager);
			Session* operator() ();
		};

	private :
		Tcp::Acceptor acceptor;
		Tcp::Connector connector;

		Pool<Session, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> session_pool;
	public:
		Address local_address;
		int port;

		std::function<void(const Address& addr)> on_connect;
		std::function<void(const Address& addr)> on_close;
	public:
		SessionManager();
		virtual ~SessionManager();

		void Listen(const Address& localRouterAddress, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose, int accept_queue_size = 5);
		void Connect(const std::string& host , int port, int timeout = 5);

		//virtual void Add(const std::shared_ptr<Network::Session>& session) override;
		//virtual void Remove(const std::shared_ptr<Network::Session>& session) override;
		virtual void OnReceive(const std::shared_ptr<Network::Session>& session, const std::shared_ptr<Buffer>& buffer) override;

		Json::Value State();
	private :
		void OnAcceptHandler(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
		void OnConnectHandler(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
	};
}}}
#endif /* ROUTERIMPL_H_ */
