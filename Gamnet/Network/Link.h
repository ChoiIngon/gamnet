#ifndef GAMNET_NETWORK_LINK_H_
#define GAMNET_NETWORK_LINK_H_

#include <deque>
#include "Session.h"
#include "../Library/Timer.h"

namespace Gamnet { namespace Network {

class LinkManager;
class Link : public std::enable_shared_from_this<Link> 
{
	std::shared_ptr<Buffer> 			read_buffer;
	std::deque<std::shared_ptr<Buffer>>	send_buffers;
public:
	struct InitFunctor
	{
		template<class T>
		T* operator() (T* link)
		{
			if (false == link->Init())
			{
				return nullptr;
			}
			return link;
		}
	};
	struct ReleaseFunctor
	{
		template<class T>
		T* operator() (T* link)
		{
			if (nullptr != link)
			{
				link->Clear();
			}
			return link;
		}
	};
	
	boost::asio::ip::tcp::socket	socket;
	boost::asio::strand				strand;
	boost::asio::ip::address 		remote_address;
	uint32_t 						link_key;
protected :
	Timer 							timer;

public :
	Link();
	virtual ~Link();

	virtual bool Init();
	virtual void Clear();
	void Connect(const char* host, int port, int timeout);
	void AsyncSend(const char* buf, int len);
	void AsyncSend(const std::shared_ptr<Buffer>& buffer);
	int  SyncSend(const char* buf, int len);
	int  SyncSend(const std::shared_ptr<Buffer>& buffer);
	virtual void Close(int reason);
		
	void OnAcceptHandler();
protected :
	virtual void OnAccept() {};
	virtual void OnConnect() {};
	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) = 0;
	virtual void OnClose(int reason) {}
private :
	void OnConnectHandler(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint);
	void OnSendHandler(const boost::system::error_code& ec, std::size_t transferredBytes);
	void AsyncRead();
	void FlushSend();
};

}}

#endif 
