#ifndef GAMNET_NETWORK_SESSION_H_
#define GAMNET_NETWORK_SESSION_H_

#include <boost/asio.hpp>
#include <atomic>
#include <map>
#include <deque>

#include "../Library/Buffer.h"
#include "../Library/Timer.h"
#include "../Log/Log.h"
#include "HandlerContainer.h"

namespace Gamnet { namespace Network {
	class SessionManager;
	class Session : public std::enable_shared_from_this<Session>
	{
	public :
		struct InitFunctor
		{
			template <class T>
			T* operator() (T* session)
			{
				if(false == session->Init())
				{
					return nullptr;
				}	
				return session;
			}
		};

		struct ReleaseFunctor
		{
			template <class T>
			T* operator() (T* session)
			{
				if(nullptr != session)
				{
					session->Clear();
				}
				return session;
			}
		};

		Session();
		virtual ~Session();

		std::shared_ptr<boost::asio::ip::tcp::socket>		socket;
		std::shared_ptr<boost::asio::strand>				strand;

		std::shared_ptr<Buffer> 							read_buffer;
		std::deque<std::shared_ptr<Buffer>>					send_buffers;

		std::map<uint32_t, std::shared_ptr<Time::Timer>>	timers;

		uint32_t											session_key;
		std::string											session_token;
		SessionManager*										session_manager;
		HandlerContainer									handler_container;
	public :
		virtual void OnCreate() = 0;
		virtual void OnAccept() = 0;
		virtual void OnClose(int reason) = 0;
		virtual void OnDestroy() = 0;

		virtual bool Init();
		virtual void Clear();
		void AsyncSend(const std::shared_ptr<Buffer> buffer);
		void AsyncSend(const char* data, size_t length);
		int SyncSend(const std::shared_ptr<Buffer> buffer);
		int SyncSend(const char* data, int length);
	
		static std::string GenerateSessionToken(uint32_t session_key);

		void OnAcceptHandler();
		void FlushSend();
		void OnSendHandler(const boost::system::error_code& ec, std::size_t transferredBytes);

		void Close(int reason);
		void AsyncRead();
		virtual void OnRead(const std::shared_ptr<Buffer>& buffer) = 0;
	};


}} /* namespace Gamnet */

#endif /* SESSION_H_ */
