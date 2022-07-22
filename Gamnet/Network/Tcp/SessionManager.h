#ifndef GAMNET_NETWORK_TCP_SESSION_MANAGER_H_
#define GAMNET_NETWORK_TCP_SESSION_MANAGER_H_

#include "Acceptor.h"
#include "CastGroup.h"
#include "Dispatcher.h"
#include "Packet.h"
#include "Session.h"
#include "../SessionManager.h"
#include "../../Library/Json/json.h"
#include "../../Library/Time/Time.h"

#include <memory>
#include <atomic>
#include <list>

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

		Acceptor		    acceptor;
		SessionPool		    session_pool;
		Time::RepeatTimer   expire_timer;
        time_t              expire_time;
	public:
		SessionManager() : session_pool(65535, SessionFactory(this)), expire_time(0)
		{
			acceptor.accept_handler = std::bind(&SessionManager::OnAcceptHandler, this, std::placeholders::_1);
		}

		void Listen(int port, int max_session, int alive_time, int accept_queue_size)
		{
			acceptor.Listen(port, max_session);
			expire_time = alive_time;
            if(0 != expire_time)
            {
                expire_timer.ExpireRepeat(60 * 1000, std::bind(&SessionManager::OnHeartBeatExpire, this));
            }
		}

		virtual void OnReceive(const std::shared_ptr<Network::Session>& session, const std::shared_ptr<Buffer>& buffer) override
		{
			Singleton<Dispatcher<SESSION_T>>::GetInstance().OnReceive(std::static_pointer_cast<SESSION_T>(session), std::static_pointer_cast<Packet>(buffer));
		}

		void DestroySession(uint32_t session_key)
		{
			std::shared_ptr<Session> session = std::static_pointer_cast<Session>(Find(session_key));
			if (nullptr == session)
			{
				LOG(WRN, "can not find session(session_key:", session_key, ")");
				return;
			}
            session->DisableHandoverSafe();
			session->Close(ErrorCode::Success);
		}

		Json::Value State()
		{
			Json::Value root;
			{
				Json::Value session;
				size_t capacity = session_pool.Capacity();
				size_t available = session_pool.Available();
				session["active_count"] = capacity - available;
				session["max_count"] = capacity;
				session["idle_count"] = available;
				root["session"] = session;
			}
			root["messages"] = Singleton<Dispatcher<SESSION_T>>::GetInstance().State();
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
			session->AsyncRead();
			session->expire_time = expire_time;
		}

        void OnHeartBeatExpire()
        {
            std::list<std::shared_ptr<Session>> delete_sessions;
            {
                std::lock_guard<std::mutex> lo(lock);
                for (const auto& pair : sessions)
                {
                    std::shared_ptr<Session> session = std::static_pointer_cast<Session>(pair.second);
                    uint64_t t = session->elaplsed_time.Count<std::chrono::seconds>();
                    if (expire_time * 3 < session->elaplsed_time.Count<std::chrono::seconds>())
                    {
                        delete_sessions.push_back(session);
                    }
                }
            }

            for (const auto& session : delete_sessions)
            {
                session->DisableHandoverSafe();
                session->Close(ErrorCode::IdleTimeoutError);
            }
        }
	};
}}}
#endif /* LISTENER_H_ */
