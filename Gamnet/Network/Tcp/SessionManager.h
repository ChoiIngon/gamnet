#ifndef GAMNET_NETWORK_TCP_SESSION_MANAGER_H_
#define GAMNET_NETWORK_TCP_SESSION_MANAGER_H_

#include <memory>
#include <atomic>
#include <mutex>

#include "../../Library/Json/json.h"
#include "../SessionManager.h"
#include "Acceptor.h"
#include "Dispatcher.h"
#include "Packet.h"
#include "Session.h"


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
		std::mutex lock;
		std::map<uint32_t, std::shared_ptr<SESSION_T>> sessions;

		Acceptor acceptor;
		
		Pool<SESSION_T, std::mutex, Network::Session::InitFunctor, Network::Session::ReleaseFunctor> session_pool;
		std::vector<std::thread> threads;
		int	expire_time; // zero means infinity
	public:
		SessionManager() : session_pool(65535, SessionFactory(this))
		{
			acceptor.accept_handler = std::bind(&SessionManager::OnAccept, this, std::placeholders::_1);
		}

		void Listen(int port, int max_session, int alive_time, int accept_queue_size, int thread_count)
		{
			acceptor.Listen(port, max_session);
			expire_time = alive_time;

			for (int i = 0; i < thread_count - 1; i++)
			{
				threads.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &(Singleton<boost::asio::io_service>::GetInstance()))));
			}
		}

		virtual void OnAccept(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) override
		{
			std::shared_ptr<SESSION_T> session = session_pool.Create();
			if (nullptr == session)
			{
				throw GAMNET_EXCEPTION(ErrorCode::InvalidSessionError, "[Gamnet::Network::Tcp] can not create session instance(availble:", session_pool.Available(), ")");
			}

			session->socket = socket;
			session->OnCreate();
			session->read_buffer = Packet::Create();
 			session->AsyncRead();
			
			std::lock_guard<std::mutex> lo(lock);
			if(false == sessions.insert(std::make_pair(session->session_key, session)).second)
			{
				assert(!"duplicated session");
				throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "duplicated session");
			}
		}

		virtual void OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket) override 
		{
		}

		virtual void OnReceive(const std::shared_ptr<Network::Session>& session, const std::shared_ptr<Buffer>& buffer) override
		{
			Singleton<Dispatcher<SESSION_T>>::GetInstance().OnReceive(std::static_pointer_cast<SESSION_T>(session), std::static_pointer_cast<Packet>(buffer));
		}

		virtual void OnDestroy(uint32_t sessionKey) override
		{
			std::lock_guard<std::mutex> lo(lock);
			sessions.erase(sessionKey);
		}
				
		std::shared_ptr<SESSION_T> Find(uint32_t sessionKey)
		{
			std::lock_guard<std::mutex> lo(lock);
			auto itr = sessions.find(sessionKey);
			if (sessions.end() == itr)
			{
				return nullptr;
			}
			return itr->second;
		}

		Json::Value State()
		{
			Json::Value root;
			root["name"] = "Gamnet::Network::Tcp";

			Json::Value session;
			session["max_count"] = session_pool.Capacity();
			session["idle_count"] = session_pool.Available();
			{
				std::lock_guard<std::mutex> lo(lock);
				session["active_count"] = sessions.size();
			}
			root["session"] = session;
			root["acceptor"] = acceptor.State();
			return root;
		}
	};
}}}
#endif /* LISTENER_H_ */
