/*
 * RouterCasterImpl.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: kukuta
 */

#include "RouterCaster.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Network { namespace Router {

bool RouterCasterImpl_Uni::RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session)
{
	if(false == mapRouteTable.insert(std::make_pair(addr, router_session)).second)
	{
		LOG(GAMNET_ERR, "[Router] register same uni-cast address(service_name:", addr.service_name.c_str(), ", id:", addr.id, ", ip:", router_session->remote_address->to_string(), ")");
		return false;
	}
	LOG(GAMNET_INF, "[Router] register uni-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ", ip:", router_session->remote_address->to_string(), ")");
	return true;
}

bool RouterCasterImpl_Uni::SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Session>& network_session, const Address& addr, const char* buf, int len)
{
	std::shared_ptr<Session> router_session = FindSession(addr);
	if(NULL == router_session)
	{
		return false;
	}
	if(NULL != network_session)
	{
		router_session->watingSessionManager_.AddSession(msg_seq, network_session);
	}
	router_session->Send(buf, len);
	return true;
}

bool RouterCasterImpl_Uni::UnregisterAddress(const Address& addr)
{
	auto itr = mapRouteTable.find(addr);
	if(mapRouteTable.end() == itr)
	{
		LOG(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}
	mapRouteTable.erase(itr);
	LOG(GAMNET_INF, "[Router] unregister uni-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ")");
	return true;
}

std::shared_ptr<Session> RouterCasterImpl_Uni::FindSession(const Address& addr)
{
	auto itr = mapRouteTable.find(addr);
	if(mapRouteTable.end() == itr)
	{
		LOG(GAMNET_WRN, "Can't find route info(service_name:", addr.service_name.c_str(), ")");
		return NULL;
	}
	return itr->second;
}

bool RouterCasterImpl_Multi::RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session)
{
	SessionList& lstSession = mapRouteTable[addr.service_name];
	for(auto&s : lstSession)
	{
		if(addr == s->addr)
		{
			LOG(GAMNET_ERR, "[Router] register same multi-cast address(service_name:", addr.service_name.c_str(), ", id:", addr.id, ", ip:", router_session->remote_address->to_string(), ")");
			return false;
		}
	}
	lstSession.push_back(router_session);
	LOG(GAMNET_INF, "[Router] register multi-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ", ip:", router_session->remote_address->to_string(), ")");
	return true;
}

bool RouterCasterImpl_Multi::SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Session>& network_session, const Address& addr, const char* buf, int len)
{
	auto itr = mapRouteTable.find(addr.service_name);
	if(mapRouteTable.end() == itr)
	{
		LOG(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	for(auto& s : itr->second)
	{
		if(NULL != network_session)
		{
			s->watingSessionManager_.AddSession(msg_seq, network_session);
		}
		s->Send(buf, len);
	}
	return true;
}

bool RouterCasterImpl_Multi::UnregisterAddress(const Address& addr)
{
	auto itr = mapRouteTable.find(addr.service_name);
	if(mapRouteTable.end() == itr)
	{
		LOG(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	SessionList& lstSession = itr->second;
	lstSession.erase(std::remove_if(lstSession.begin(), lstSession.end(), [&addr](const std::shared_ptr<Session> session) -> bool {
		if(session->addr.id == addr.id)
		{
			LOG(GAMNET_INF, "[Router] unregister multi-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ", ip:", session->remote_address->to_string(), ")");
			return true;
		}
		return false;
	}));
	return true;
}

bool RouterCasterImpl_Any::RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session)
{
	std::pair<int, SessionArray>& pairSessionArray = mapRouteTable[addr.service_name];
	SessionArray& arrSession = pairSessionArray.second;
	for(auto&s : arrSession)
	{
		if(addr == s->addr)
		{
			LOG(GAMNET_ERR, "[Router] register same any-cast address(service_name:", addr.service_name.c_str(), ", id:", addr.id, ", ip:", router_session->remote_address->to_string(), ")");
			return false;
		}
	}

	arrSession.push_back(router_session);
	pairSessionArray.first = arrSession.size()-1;
	LOG(GAMNET_INF, "[Router] register any-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ", ip:", router_session->remote_address->to_string(), ")");
	return true;
}

bool RouterCasterImpl_Any::SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Session>& network_session, const Address& addr, const char* buf, int len)
{
	auto itr = mapRouteTable.find(addr.service_name);
	if(mapRouteTable.end() == itr)
	{
		LOG(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	std::pair<int, SessionArray>& pairSessionArray = itr->second;
	SessionArray& arrSession = pairSessionArray.second;

	if(0 >= arrSession.size())
	{
		LOG(GAMNET_ERR, "Cant find Session");
		return false;
	}
	std::shared_ptr<Session> router_session = arrSession[pairSessionArray.first++ % arrSession.size()];
	if(NULL != network_session)
	{
		router_session->watingSessionManager_.AddSession(msg_seq, network_session);
	}

	return router_session->Send(buf, len);
}

bool RouterCasterImpl_Any::UnregisterAddress(const Address& addr)
{
	auto itr = mapRouteTable.find(addr.service_name);
	if(mapRouteTable.end() == itr)
	{
		LOG(GAMNET_ERR, "Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	std::pair<int, SessionArray>& pairSessionArray = itr->second;
	SessionArray& arrSession = pairSessionArray.second;
	arrSession.erase(std::remove_if(arrSession.begin(), arrSession.end(), [&addr](const std::shared_ptr<Session> session) -> bool {
		if(session->addr.id == addr.id)
		{
			LOG(GAMNET_INF, "[Router] unregister any-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ", ip:", session->remote_address->to_string(), ")");
			return true;
		}
		return false;
	}));
	return true;
}

RouterCaster::RouterCaster() 
{
	msgseq_ = 1;
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

bool RouterCaster::SendMsg(std::shared_ptr<Network::Session> network_session, const Address& addr, const char* buf, int len)
{
	MsgRouter_SendMsg_Ntf ntf;
	ntf.nKey = addr.msg_seq;
	if(NULL != network_session)
	{
		ntf.nKey = msgseq_++;
	}
	ntf.sBuf.assign(buf, len);
	std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
	if(NULL == packet)
	{
		return false;
	}
	if(false == packet->Write(ntf))
	{
		return false;
	}
	if(ROUTER_CAST_MAX <= addr.cast_type)
	{
		LOG(GAMNET_ERR, addr.cast_type, " is undefined cast_type");
		return false;
	}
	std::lock_guard<std::mutex> lo(lock_);
	return arrCasterImpl_[addr.cast_type]->SendMsg(ntf.nKey, network_session, addr, packet->ReadPtr(), packet->Size());
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
}}}

