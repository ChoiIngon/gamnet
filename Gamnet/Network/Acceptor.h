#ifndef GAMNET_NETWORK_ACCEPTOR_H_
#define GAMNET_NETWORK_ACCEPTOR_H_

#include "../Library/Pool.h"
#include "../Library/Debugs.h"
#include "../Library/Delegate.h"
#include "Link.h"

namespace Gamnet { namespace Network {
	class Acceptor
	{
		struct SocketFactory
		{
			boost::asio::ip::tcp::socket* operator() ()
			{
				return new boost::asio::ip::tcp::socket(Gamnet::Singleton<boost::asio::io_service>::GetInstance());
			}
		};
		
		struct SocketInitFunctor
		{
			boost::asio::ip::tcp::socket* operator() (boost::asio::ip::tcp::socket* socket)
			{
				return socket;
			}
		};

		struct SocketReleaseFunctor
		{
			Acceptor& acceptor;
			SocketReleaseFunctor(Acceptor& acceptor) : acceptor(acceptor)
			{
			}

			boost::asio::ip::tcp::socket* operator() (boost::asio::ip::tcp::socket* socket)
			{
				acceptor.Release();
				return socket;
			}
		};
	private:
		boost::asio::ip::tcp::acceptor	acceptor;
		boost::asio::ip::tcp::endpoint	endpoint;
		Pool<boost::asio::ip::tcp::socket, std::mutex, SocketInitFunctor, SocketReleaseFunctor> socket_pool;

		int								max_queue_size;
		std::atomic<int>				cur_queue_size;
	public:
		Acceptor(int nSize = 65535);

		Delegate<void(std::shared_ptr<boost::asio::ip::tcp::socket>& socket)> on_accept;
		void Listen(int port, int max_count, int queue_size);
		void Release();

	private:
		void Accept();
		void Callback_Accept(const std::shared_ptr<SESSION_T> session, const boost::system::error_code& ec);
	};

	template<class SESSION_T>
	Acceptor<SESSION_T>::Acceptor(int nSize) :
		acceptor(Singleton<boost::asio::io_service>::GetInstance()),
		session_pool(),
		socket_pool(nSize, Acceptor<SESSION_T>::SocketFactory(), SocketInitFunctor(), SocketReleaseFunctor(*this)),
		max_queue_size(0),
		cur_accept_count(0)
	{
	}

	template<class SESSION_T>
	void Acceptor<SESSION_T>::Listen(int port, int max_count, int queue_size)
	{
		socket_pool.Capacity(max_count);
		endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor.open(endpoint.protocol());
		acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor.bind(endpoint);
		acceptor.listen();
		session_manager.Init();
		max_queue_size = queue_size;
		for (int i = 0; i < this->max_queue_size; i++)
		{
			cur_queue_size++;
			Accept();
		}
	}

	template<class SESSION_T>
	void Acceptor<SESSION_T>::Release()
	{
		if (cur_queue_size < max_queue_size)
		{
			cur_queue_size++;
			Accept();
		}
	}

	template<class SESSION_T>
	void Acceptor<SESSION_T>::Accept()
	{
		std::shared_ptr<boost::asio::ip::tcp::socket> socket = socket_pool.Create();
		if (nullptr == socket)
		{
			LOG(GAMNET_ERR, "can not create socket. deny addtional connection");
			cur_queue_size = std::max(cur_accept_count - 1, 0);
			return;
		}

		std::shared_ptr<SESSION_T> session = session_pool.Create();
		if (nullptr == session)
		{
			LOG(GAMNET_ERR, "can not create session. deny addtional connection");
			return;
		}

		session->socket = socket;
		acceptor.async_accept(*socket, boost::bind(&Acceptor2::Callback_Accept, this, session, boost::asio::placeholders::error));
	}

	template<class SESSION_T>
	void Acceptor<SESSION_T>::Callback_Accept(const std::shared_ptr<SESSION_T> session, const boost::system::error_code& ec)
	{
		Accept();

		try {
			if (0 != ec)
			{
				throw GAMNET_EXCEPTION(ErrorCode::AcceptFailError, "session_key:", session->session_key, ", error_code:", ec.value());
			}

			session_manager.Add(session);
			on_accept(socket);
			session->OnAcceptHandler();
			return;
		}
		catch (const Exception& e)
		{
			LOG(GAMNET_ERR, "accept fail(errno:", e.error_code(), ")");
		}
		catch (const boost::system::system_error& e)
		{
			LOG(GAMNET_ERR, "accept fail(errno:", e.code().value(), ", errstr:", e.what(), ")");
		}
	}

	template<class SESSION_T>
	bool Acceptor<SESSION_T>::SessionManager::Add(const std::shared_ptr<SESSION_T>& session)
	{
		std::lock_guard<std::mutex> lo(lock);
		if (false == sessions.insert(std::make_pair(session->session_key, session)).second)
		{
			LOG(GAMNET_ERR, "[session_key:", session->session_key, "] duplicated session key");
			assert(!"duplicate session");
			return false;
		}

		return true;
	}
	
	template<class SESSION_T>
	void Acceptor<SESSION_T>::SessionManager::Remove(const std::shared_ptr<SESSION_T>& session)
	{
		std::lock_guard<std::mutex> lo(lock);
		sessions.erase(key);
	}
	
	template<class SESSION_T>
	std::shared_ptr<SESSION_T> Acceptor<SESSION_T>::SessionManager::Find(uint32_t key)
	{
		std::lock_guard<std::mutex> lo(lock);
		auto itr = sessions.find(key);
		if (sessions.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	template<class SESSION_T>
	size_t Acceptor<SESSION_T>::SessionManager::Size()
	{
		std::lock_guard<std::mutex> lo(lock);
		return sessions.size();
	}
}}
#endif
