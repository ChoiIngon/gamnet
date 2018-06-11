#ifndef GAMNET_NETWORK_ROUTER_CASTER_H_
#define GAMNET_NETWORK_ROUTER_CASTER_H_

#include <list>
#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "Session.h"

namespace Gamnet { namespace Network {namespace Router {

struct RouterCasterImpl {
	RouterCasterImpl() {}
	virtual ~RouterCasterImpl() {}
	virtual bool RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session) = 0;
	virtual bool SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Tcp::Session>& network_session,  const Address& addr, const std::shared_ptr<Tcp::Packet>& packet) = 0;
	virtual bool UnregisterAddress(const Address& addr) = 0;
};

struct RouterCasterImpl_Uni : public RouterCasterImpl
{
private :
	std::mutex lock_;
	std::map<Address, std::shared_ptr<Session>> route_table_;

public :
	virtual bool RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session) override;
	virtual bool SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Tcp::Session>& network_session, const Address& addr, const std::shared_ptr<Tcp::Packet>& packet) override;
	virtual bool UnregisterAddress(const Address& addr) override;
	std::shared_ptr<Session> FindSession(const Address& addr);
};

struct RouterCasterImpl_Multi : public RouterCasterImpl
{
private :
	std::mutex lock_;
	std::map<std::string, std::list<std::shared_ptr<Session>>> route_table_;

public :
	virtual bool RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session) override;
	virtual bool SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Tcp::Session>& network_session, const Address& addr, const std::shared_ptr<Tcp::Packet>& packet) override;
	virtual bool UnregisterAddress(const Address& addr) override;
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
	virtual bool SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Tcp::Session>& network_session, const Address& addr, const std::shared_ptr<Tcp::Packet>& packet) override;
	virtual bool UnregisterAddress(const Address& addr) override;
};

struct RouterCaster
{
private :
	std::atomic<uint32_t> msg_seq;
	std::shared_ptr<RouterCasterImpl> arrCasterImpl_[ROUTER_CAST_TYPE::MAX];

public :
	RouterCaster();
	bool RegisterAddress(const Address& addr, std::shared_ptr<Session> router_session);
	bool SendMsg(const std::shared_ptr<Network::Tcp::Session>& network_session, const Address& addr, const char* buf, int len);
	bool UnregisterAddress(const Address& addr);
	std::shared_ptr<Session> FindSession(const Address& addr);
};
}}}
#endif /* ROUTER_CASTER_H_ */
