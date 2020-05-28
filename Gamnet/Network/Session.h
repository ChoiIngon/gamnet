#ifndef GAMNET_NETWORK_SESSION_H_
#define GAMNET_NETWORK_SESSION_H_

#include <boost/asio.hpp>
#include <deque>
#include "../Library/Buffer.h"

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
				if (false == session->Init())
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
				if (nullptr != session)
				{
					session->Clear();
				}
				return session;
			}
		};

		Session();
		virtual ~Session();

		uint32_t											session_key;
		SessionManager*										session_manager;

		std::shared_ptr<boost::asio::ip::tcp::socket>		socket;
		std::shared_ptr<boost::asio::strand>				strand;
		std::shared_ptr<Buffer> 							read_buffer;
		std::deque<std::shared_ptr<Buffer>>					send_buffers;
	public :
		virtual void OnCreate() = 0;
		virtual void OnAccept() = 0;
		virtual void OnClose(int reason) = 0;
		virtual void OnDestroy() = 0;
		virtual void OnRead(const std::shared_ptr<Buffer>& buffer) = 0;

		void AsyncSend(const char* data, size_t length);
		void AsyncSend(const std::shared_ptr<Buffer>& buffer);
		int SyncSend(const char* data, int length);
		int SyncSend(const std::shared_ptr<Buffer>& buffer);
		
		virtual void Close(int reason);
		void AsyncRead();
	protected :
		virtual bool Init();
		virtual void Clear();
	
		void FlushSend();
	private :
		void OnSendHandler(const boost::system::error_code& ec, std::size_t transferredBytes);
	};
}} /* namespace Gamnet */

#endif /* SESSION_H_ */
