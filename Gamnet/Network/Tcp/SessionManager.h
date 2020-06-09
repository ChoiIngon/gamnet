#ifndef GAMNET_NETWORK_TCP_SESSION_MANAGER_H_
#define GAMNET_NETWORK_TCP_SESSION_MANAGER_H_

#include <memory>
#include <atomic>


#include "../../Library/Json/json.h"
#include "../SessionManager.h"
#include "Acceptor.h"
#include "Dispatcher.h"
#include "Packet.h"
#include "Session.h"
#include "CastGroup.h"

namespace Gamnet { namespace Network { namespace Tcp {

	template <class SESSION_T>
	class SessionManager : public Network::SessionManager
	{
	public:
		struct SessionFactory
		{
			SessionManager* const session_manager;
			SessionFactory(SessionManager* manager) : session_manager(manager)
			{
			}

			SESSION_T* operator() ()
			{
				SESSION_T* session = new SESSION_T();
				session->session_manager = session_manager;
				return session;
			}
		};

	private:
		typedef Pool<SESSION_T, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> SessionPool;

		Acceptor		acceptor;
		SessionPool		session_pool;
		int				expire_time; // zero means infinity

	public:
		SessionManager() : session_pool(65535, SessionFactory(this))
		{
			acceptor.accept_handler = std::bind(&SessionManager::OnAcceptHandler, this, std::placeholders::_1);
		}

		void Listen(int port, int max_session, int alive_time, int accept_queue_size)
		{
			expire_time = alive_time;
			acceptor.Listen(port, max_session);
		}
		
		virtual void OnReceive(const std::shared_ptr<Network::Session>& session, const std::shared_ptr<Buffer>& buffer) override
		{
			Singleton<Dispatcher<SESSION_T>>::GetInstance().OnReceive(std::static_pointer_cast<SESSION_T>(session), std::static_pointer_cast<Packet>(buffer));
		}

		Json::Value State()
		{
			Json::Value root;
			root["name"] = "Gamnet::Network::Tcp";

			Json::Value session;
			session["max_count"] = session_pool.Capacity();
			session["idle_count"] = session_pool.Available();
			{
				//std::lock_guard<std::mutex> lo(lock);
				//session["active_count"] = sessions.size();
			}
			root["session"] = session;
			root["acceptor"] = acceptor.State();
			return root;
		}
	private :
		void OnAcceptHandler(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
		{
			std::shared_ptr<SESSION_T> session = session_pool.Create();
			if (nullptr == session)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionError, "[Gamnet::Network::Tcp] can not create session instance(availble:", session_pool.Available(), ")");
			}

			session->socket = socket;
			session->OnCreate();
			session->AsyncRead();
			session->OnAccept();
			Add(session);
		}
	};
}}}
#endif /* LISTENER_H_ */
