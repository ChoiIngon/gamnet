/*
 * RouterCasterImpl.h
 *
 *  Created on: Jun 19, 2013
 *      Author: kukuta
 */

#ifndef ROUTER_CASTER_H_
#define ROUTER_CASTER_H_

#include <list>
#include <string>
#include <map>
#include <vector>
#include <mutex>
#include "Session.h"
#include "MsgRouter.h"

namespace Gamnet { namespace Router {

struct RouterCasterImpl {
	virtual ~RouterCasterImpl() {}
	virtual bool RegisterAddress(const Address& addr, std::shared_ptr<Session> session) = 0;
	virtual bool SendMsg(const Address& addr, const char* buf, int len) = 0;
	virtual bool UnregisterAddress(const Address& addr) = 0;
};

struct RouterCasterImpl_Uni : public RouterCasterImpl
{
	typedef std::map<Address, std::shared_ptr<Session>> RoutingTableMap;
	RoutingTableMap mapRouteTable;

	virtual bool RegisterAddress(const Address& addr, std::shared_ptr<Session> session);
	virtual bool SendMsg(const Address& addr, const char* buf, int len);
	virtual bool UnregisterAddress(const Address& addr);

	std::shared_ptr<Session> FindSession(const Address& addr);
};

struct RouterCasterImpl_Multi : public RouterCasterImpl
{
	typedef std::list<std::shared_ptr<Session>> SessionList;
	typedef std::map<std::string, SessionList> RoutingTableMap;
	RoutingTableMap mapRouteTable;

	virtual bool RegisterAddress(const Address& addr, std::shared_ptr<Session> session);
	virtual bool SendMsg(const Address& addr, const char* buf, int len);
	virtual bool UnregisterAddress(const Address& addr);
};

struct RouterCasterImpl_Any : public RouterCasterImpl
{
	typedef std::vector<std::shared_ptr<Session>> SessionArray;
	typedef std::map<std::string, std::pair<int, SessionArray> > RoutingTableMap;
	RoutingTableMap mapRouteTable;

	virtual bool RegisterAddress(const Address& addr, std::shared_ptr<Session> session);
	virtual bool SendMsg(const Address& addr, const char* buf, int len);
	virtual bool UnregisterAddress(const Address& addr);
};

struct RouterCaster
{
	std::mutex lock_;
	std::shared_ptr<RouterCasterImpl> arrCasterImpl_[ROUTER_CAST_MAX];
	RouterCaster();
	bool RegisterAddress(const Address& addr, std::shared_ptr<Session> session);
	bool SendMsg(const Address& addr, const char* buf, int len);
	bool UnregisterAddress(const Address& addr);
	std::shared_ptr<Session> FindSession(const Address& addr);
};
}};
#endif /* ROUTER_CASTER_H_ */
