#ifndef GAMNET_NETWORK_ROUTER_CASTER_H_
#define GAMNET_NETWORK_ROUTER_CASTER_H_

#include <list>
#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <atomic>
#include "Session.h"
#include "../../Library/Time/Timer.h"
#include "Address.h"

namespace Gamnet { namespace Network {namespace Router {

struct RouterCasterImpl {
	RouterCasterImpl() {}
	virtual ~RouterCasterImpl() {}
	virtual bool RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session) = 0;
	virtual int  SendMsg(const Address& addr, const std::shared_ptr<Tcp::Packet>& packet) = 0;
	virtual bool UnregisterAddress(const Address& addr) = 0;
	virtual std::shared_ptr<Session> FindSession(const Address& addr) = 0;
};

struct RouterCasterImpl_Uni : public RouterCasterImpl
{
private :
	std::mutex lock_;
	std::map<Address, std::shared_ptr<Session>> route_table_;

	static constexpr int HEARTBEAT_INTERVAL = 60 * 1000; //ms
	Time::Timer heartbeat_timer;
public :
	RouterCasterImpl_Uni();
	virtual bool RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session) override;
	virtual int  SendMsg(const Address& addr, const std::shared_ptr<Tcp::Packet>& packet) override;
	virtual bool UnregisterAddress(const Address& addr) override;
	virtual std::shared_ptr<Session> FindSession(const Address& addr) override;
};

struct RouterCasterImpl_Multi : public RouterCasterImpl
{
private :
	std::mutex lock_;
	std::map<std::string, std::list<std::shared_ptr<Session>>> route_table_;

public :
	virtual bool RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session) override;
	virtual int  SendMsg(const Address& addr, const std::shared_ptr<Tcp::Packet>& packet) override;
	virtual bool UnregisterAddress(const Address& addr) override;
	virtual std::shared_ptr<Session> FindSession(const Address& addr) override;
};

struct RouterCasterImpl_Any : public RouterCasterImpl
{
private :
	typedef std::vector<std::shared_ptr<Session>> SessionArray;
	typedef std::map<std::string, std::pair<int, SessionArray> > RoutingTableMap;

	std::mutex lock_;
	RoutingTableMap route_table_;
public :
	virtual bool RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session) override;
	virtual int  SendMsg(const Address& addr, const std::shared_ptr<Tcp::Packet>& packet) override;
	virtual bool UnregisterAddress(const Address& addr) override;
	virtual std::shared_ptr<Session> FindSession(const Address& addr) override;
};

struct RouterCaster
{
private :
	std::shared_ptr<RouterCasterImpl> arrCasterImpl_[(int)Address::CAST_TYPE::MAX];

public :
	RouterCaster();
	bool RegisterAddress(const Address& addr, std::shared_ptr<Session> router_session);
	int  SendMsg(const Address& addr, const std::shared_ptr<Buffer>& buffer);
	bool UnregisterAddress(const Address& addr);
	std::shared_ptr<Session> FindSession(const Address& addr);
};
}}}
#endif /* ROUTER_CASTER_H_ */
