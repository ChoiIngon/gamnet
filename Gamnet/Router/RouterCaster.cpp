/*
 * RouterCasterImpl.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: kukuta
 */

#include "RouterCaster.h"
#include "../Log/Log.h"

namespace Gamnet { namespace Router {

bool RouterCasterImpl_Uni::RegisterAddress(const Address& addr, std::shared_ptr<Session> session)
{
	if(false == mapRouteTable.insert(std::make_pair(addr, session)).second)
	{
		Log::Write(GAMNET_ERR, "register same address(service_name:", addr.service_name.c_str(), ", cast_type:", addr.cast_type, ", id:", addr.id, ")");
		return false;
	}
	Log::Write(GAMNET_INF, "register unicast address ok(service_name:", addr.service_name.c_str(), ", id:", addr.id, ")");
	return true;
}

bool RouterCasterImpl_Uni::SendMsg(const Address& addr, const char* buf, int len)
{
	std::shared_ptr<Session> session = FindSession(addr);
	if(NULL == session)
	{
		return false;
	}
	session->Send(buf, len);
	Log::Write(GAMNET_DEV, "send uni_cast(dest_addr:{service_name:", addr.service_name.c_str(), ", id:", addr.id, "}, transferred_bytes:", len, ")");
	return true;
}

bool RouterCasterImpl_Uni::UnregisterAddress(const Address& addr)
{
	auto itr = mapRouteTable.find(addr);
	if(mapRouteTable.end() == itr)
	{
		Log::Write(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}
	mapRouteTable.erase(itr);
	Log::Write(GAMNET_DEV, "Unregister Unicast Address(service_name:", addr.service_name, ", id:", addr.id, ")");;
	return true;
}

std::shared_ptr<Session> RouterCasterImpl_Uni::FindSession(const Address& addr)
{
	auto itr = mapRouteTable.find(addr);
	if(mapRouteTable.end() == itr)
	{
		Log::Write(GAMNET_WRN, "Can't find route info(service_name:", addr.service_name.c_str(), ")");
		return NULL;
	}
	return itr->second;
}

bool RouterCasterImpl_Multi::RegisterAddress(const Address& addr, std::shared_ptr<Session> session)
{
	SessionList& lstSession = mapRouteTable[addr.service_name];
	for(auto&s : lstSession)
	{
		if(addr == s->addr)
		{
			Log::Write(GAMNET_ERR, "register same address(service_name:", addr.service_name.c_str(), ", cast_type:", addr.cast_type, ", id:", addr.id, ")");
			return false;
		}
	}
	lstSession.push_back(session);
	Log::Write(GAMNET_INF, "register multicast address ok(service_name:", addr.service_name.c_str(), ", )");
	return true;
}

bool RouterCasterImpl_Multi::SendMsg(const Address& addr, const char* buf, int len)
{
	auto itr = mapRouteTable.find(addr.service_name);
	if(mapRouteTable.end() == itr)
	{
		Log::Write(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	for(auto& s : itr->second)
	{
		s->Send(buf, len);
	}
	Log::Write(GAMNET_DEV, "send multi_cast(dest_addr:service_name:", addr.service_name.c_str(), ", transferred_bytes:", len, ")");
	return true;
}

bool RouterCasterImpl_Multi::UnregisterAddress(const Address& addr)
{
	auto itr = mapRouteTable.find(addr.service_name);
	if(mapRouteTable.end() == itr)
	{
		Log::Write(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	SessionList& lstSession = itr->second;
	lstSession.erase(std::remove_if(lstSession.begin(), lstSession.end(), [&addr](const std::shared_ptr<Session> session) -> bool {
		if(session->addr.id == addr.id)
		{
			Log::Write(GAMNET_DEV, "Unregister Multicast Address(service_name:", addr.service_name, ", id:", addr.id, ")");;
			return true;
		}
		return false;
	}));
	return true;
}

bool RouterCasterImpl_Any::RegisterAddress(const Address& addr, std::shared_ptr<Session> session)
{
	std::pair<int, SessionArray>& pairSessionArray = mapRouteTable[addr.service_name];
	SessionArray& arrSession = pairSessionArray.second;
	for(auto&s : arrSession)
	{
		if(addr == s->addr)
		{
			Log::Write(GAMNET_ERR, "register same address(service_name:", addr.service_name.c_str(), ", cast_type:", addr.cast_type, ", id:", addr.id, ")");
			return false;
		}
	}

	arrSession.push_back(session);
	pairSessionArray.first = arrSession.size()-1;
	Log::Write(GAMNET_INF, "register anycast address ok(service_name:", addr.service_name.c_str(), ")");
	return true;
}

bool RouterCasterImpl_Any::SendMsg(const Address& addr, const char* buf, int len)
{
	auto itr = mapRouteTable.find(addr.service_name);
	if(mapRouteTable.end() == itr)
	{
		Log::Write(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	std::pair<int, SessionArray>& pairSessionArray = itr->second;
	SessionArray& arrSession = pairSessionArray.second;

	if(0 >= arrSession.size())
	{
		Log::Write(GAMNET_ERR, "Cant find Session");
		return false;
	}
	Log::Write(GAMNET_DEV, "send multi_cast(dest_addr:service_name:", addr.service_name.c_str(), ", transferred_bytes:", len, ")");
	return arrSession[pairSessionArray.first++ % arrSession.size()]->Send(buf, len);
}

bool RouterCasterImpl_Any::UnregisterAddress(const Address& addr)
{
	auto itr = mapRouteTable.find(addr.service_name);
	if(mapRouteTable.end() == itr)
	{
		Log::Write(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	std::pair<int, SessionArray>& pairSessionArray = itr->second;
	SessionArray& arrSession = pairSessionArray.second;
	arrSession.erase(std::remove_if(arrSession.begin(), arrSession.end(), [&addr](const std::shared_ptr<Session> session) -> bool {
		if(session->addr.id == addr.id)
		{
			Log::Write(GAMNET_DEV, "Unregister Anycast Address(service_name:", addr.service_name, ", id:", addr.id, ")");;
			return true;
		}
		return false;
	}));
	return true;
}

RouterCaster::RouterCaster()
{
	arrCasterImpl_[ROUTER_CAST_UNI] = std::shared_ptr<RouterCasterImpl>(new RouterCasterImpl_Uni());
	arrCasterImpl_[ROUTER_CAST_MULTI] = std::shared_ptr<RouterCasterImpl>(new RouterCasterImpl_Multi());
	arrCasterImpl_[ROUTER_CAST_ANY] = std::shared_ptr<RouterCasterImpl>(new RouterCasterImpl_Any());
}

bool RouterCaster::RegisterAddress(const Address& addr, std::shared_ptr<Session> session)
{
	std::lock_guard<std::mutex> lo(lock_);
	for(int i=0; i<ROUTER_CAST_MAX; i++)
	{
		if(false == arrCasterImpl_[i]->RegisterAddress(addr, session))
		{
			return false;
		}
	}
	session->addr = addr;
	return true;
}

bool RouterCaster::SendMsg(const Address& addr, const char* buf, int len)
{
	std::lock_guard<std::mutex> lo(lock_);
	if(ROUTER_CAST_MAX <= addr.cast_type)
	{
		Log::Write(GAMNET_ERR, addr.cast_type, " is undefined cast_type");
		return false;
	}
	return arrCasterImpl_[addr.cast_type]->SendMsg(addr, buf, len);
}

bool RouterCaster::UnregisterAddress(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	for(int i=0; i<ROUTER_CAST_MAX; i++)
	{
		if(false == arrCasterImpl_[i]->UnregisterAddress(addr))
		{
			return false;
		}
	}
	return true;
}

std::shared_ptr<Session> RouterCaster::FindSession(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	std::shared_ptr<RouterCasterImpl_Uni> caster_impl = std::static_pointer_cast<RouterCasterImpl_Uni>(arrCasterImpl_[ROUTER_CAST_UNI]);
	return caster_impl->FindSession(addr);
}
}};

