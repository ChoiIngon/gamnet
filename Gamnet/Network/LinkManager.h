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
	std::string _name;
	//Timer 		_timer;
	uint32_t	_keepalive_time;

	std::recursive_mutex _lock;
	//std::map<uint64_t, std::shared_ptr<Link>> _links;

	volatile bool _is_acceptable;
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::endpoint _endpoint;
public :
	static std::atomic_ulong link_key;

	LinkManager();
	virtual ~LinkManager();

	void Listen(int port, int max_session, int keep_time);

	virtual void OnAccept(const std::shared_ptr<Link>& link);
	virtual void OnConnect(const std::shared_ptr<Link>& link);

	virtual void OnClose(const std::shared_ptr<Link>& link, int reason);
	virtual void OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer) = 0;

	virtual std::shared_ptr<Link> Create() = 0;
	virtual size_t	Available();
	virtual size_t	Size();
	virtual size_t	Capacity() const;
	//virtual void	Capacity(size_t count);

	//bool Add(uint64_t key, const std::shared_ptr<Link>& link);
	//void Remove(uint64_t key);
	//std::shared_ptr<Link> Find(uint64_t key);

	Json::Value State();
private :
	void Accept();
	void Callback_Accept(const std::shared_ptr<Link>& link, const boost::system::error_code& error);
};
}} /* namespace Gamnet */

#endif /* NETWORK_LISTENER_H_ */
