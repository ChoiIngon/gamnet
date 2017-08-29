#ifndef GAMNET_NETWORK_LINK_H_
#define GAMNET_NETWORK_LINK_H_

#include "Session.h"
#include <deque>

namespace Gamnet { namespace Network {

class LinkManager;
class Link : public std::enable_shared_from_this<Link> {
public:
	struct Init
	{
		Link* operator() (Link* link)
		{
			link->link_key = 0;
			link->msg_seq = 0;
			link->expire_time = time(NULL);
			link->manager = NULL;
			link->read_buffer = Buffer::Create();
			link->send_buffers.clear();
			link->session = NULL;
			return link;
		}
	};
	boost::asio::ip::tcp::socket socket;
	boost::asio::strand strand;
	boost::asio::ip::address remote_address;
	Timer 		timer;
	uint32_t 	link_key;
	uint32_t 	msg_seq;
	time_t 		expire_time;

	std::shared_ptr<Buffer> 			read_buffer;
	std::deque<std::shared_ptr<Buffer>>	send_buffers;
	std::shared_ptr<Session> session;
private :
	LinkManager* 	manager;

public :
	Link();
	virtual ~Link();

	virtual void Connect(const char* host, int port, int timeout);
	virtual void OnError(int reason);
	virtual void AsyncRead();

	void AsyncSend(const std::shared_ptr<Buffer>& buffer);
	void AsyncSend(const char* buf, int len);
	int SyncSend(const std::shared_ptr<Buffer>& buffer);
	int SyncSend(const char* buf, int len);

	void AttachManager(LinkManager* manager);
	void AttachSession(const std::shared_ptr<Session> session);
private :
	void FlushSend();
};

}
} /* namespace Gamnet */

#endif /* NETWORK_Link_H_ */
