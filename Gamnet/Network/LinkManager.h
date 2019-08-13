#ifndef GAMNET_NETWORK_LINKMANAGER_H_
#define GAMNET_NETWORK_LINKMANAGER_H_

#include <atomic>
#include <mutex>
#include <boost/bind.hpp>

#include "Link.h"
#include "../Library/Pool.h"
#include "../Library/Json/json.h"

namespace Gamnet { namespace Network {

class LinkManager
{
	std::mutex	_lock;
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::endpoint _endpoint;
	std::map<uint32_t, std::shared_ptr<Network::Link>> _links;

	int _max_accept_size;
	int _keep_alive_time;
	volatile int _cur_accept_size;
public :
	std::string	name;
public :
	LinkManager();
	virtual ~LinkManager();

	bool Listen(int port, int accept_queue_size, int keep_alive_time);
	virtual std::shared_ptr<Link> Connect(const char* host, int port, int timeout);

	virtual void OnAccept(const std::shared_ptr<Link>& link) {}
	virtual void OnConnect(const std::shared_ptr<Link>& link) {}
	virtual void OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer) {};
	virtual void OnClose(const std::shared_ptr<Link>& link, int reason) {}	

	virtual std::shared_ptr<Link> Create() = 0;
	bool Add(const std::shared_ptr<Link>& link);
	void Remove(uint32_t linkKey);
	size_t Size();
private :
	bool Accept();
	void Callback_Accept(const std::shared_ptr<Link> link, const boost::system::error_code& error);
	Timer expire_timer;
	void OnTimerExpire();
};

}} /* namespace Gamnet */

#endif /* NETWORK_LISTENER_H_ */
