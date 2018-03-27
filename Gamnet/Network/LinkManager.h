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
protected:
	std::string	_name;
	std::mutex	_lock;
	volatile bool _is_acceptable;
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::endpoint _endpoint;
public :
	static std::atomic_ulong link_key;

	LinkManager();
	virtual ~LinkManager();

	void Listen(int port);
	virtual std::shared_ptr<Link> Connect(const char* host, int port, int timeout);
	void Close();

	virtual void OnAccept(const std::shared_ptr<Link>& link) {}
	virtual void OnConnect(const std::shared_ptr<Link>& link) {}
	virtual void OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer) {};
	virtual void OnClose(const std::shared_ptr<Link>& link, int reason) {}	

	virtual std::shared_ptr<Link> Create() = 0;
	virtual size_t	Available();
	virtual size_t	Size();
	virtual size_t	Capacity();
	
	Json::Value State();
private :
	void Accept();
	void Callback_Accept(const std::shared_ptr<Link>& link, const boost::system::error_code& error);
};
}} /* namespace Gamnet */

#endif /* NETWORK_LISTENER_H_ */
