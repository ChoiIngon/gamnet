#ifndef GAMNET_NETWORK_LINK_H_
#define GAMNET_NETWORK_LINK_H_

#include "Session.h"
#include <deque>

namespace Gamnet { namespace Network {

class LinkManager;
class Link : public std::enable_shared_from_this<Link> 
{
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

	boost::asio::ip::tcp::socket socket;
	boost::asio::strand	strand;
	boost::asio::ip::address remote_address;

	Timer 		timer;
	uint32_t 	link_key;
	std::shared_ptr<Session> session;
	LinkManager* const link_manager;
private :
	std::shared_ptr<Buffer> 			read_buffer;
	std::deque<std::shared_ptr<Buffer>>	send_buffers;
public :
	Link(LinkManager* linkManager);
	virtual ~Link();

	virtual bool Init();
	void Connect(const char* host, int port, int timeout);
	void AsyncSend(const char* buf, int len);
	void AsyncSend(const std::shared_ptr<Buffer>& buffer);
	int  SyncSend(const char* buf, int len);
	int  SyncSend(const std::shared_ptr<Buffer>& buffer);
	void Close(int reason);
		
	void AttachSession(const std::shared_ptr<Session> session);

	void AsyncRead();
protected :
	virtual void OnRead(const std::shared_ptr<Buffer>& buffer) = 0;
private :
	void FlushSend();
};

}}

#endif 
