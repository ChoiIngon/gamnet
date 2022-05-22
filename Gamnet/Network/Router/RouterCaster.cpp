/*
 * RouterCasterImpl.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: kukuta
 */

#include "RouterCaster.h"
#include "MsgRouter.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Network { namespace Router {

RouterCasterImpl_Uni::RouterCasterImpl_Uni()
{
	heartbeat_timer.ExpireRepeat(HEARTBEAT_INTERVAL, [this]() {
		std::lock_guard<std::mutex> lo(lock_);
		MsgRouter_HeartBeat_Ntf ntf;
		std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
		if (nullptr == packet)
		{
			return;
		}
		if (false == packet->Write(ntf))
		{
			return;
		}
		for (auto itr : route_table_)
		{
			std::shared_ptr<Session> session = itr.second;
			session->AsyncSend(packet);
		}
		LOG(GAMNET_INF, "[Gamnet::Network::Router] send heartbeat message(connected server count:", route_table_.size(), ")");
	});
}
bool RouterCasterImpl_Uni::RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session)
{
	std::lock_guard<std::mutex> lo(lock_);
	if(false == route_table_.insert(std::make_pair(addr, router_session)).second)
	{
		LOG(INF, "[Gamnet::Network::Router] duplicated uni-cast address(address:", addr.ToString(), ")");
		return false;
	}
	LOG(INF, "[Gamnet::Network::Router] register uni-cast address success(address:", addr.ToString(), ")");
	return true;
}

int RouterCasterImpl_Uni::SendMsg(const Address& addr, const std::shared_ptr<Tcp::Packet>& packet)
{
	std::shared_ptr<Session> router_session = FindSession(addr);
	if(nullptr == router_session)
	{
		return 0;
	}
	router_session->AsyncSend(packet);
	return 1;
}

bool RouterCasterImpl_Uni::UnregisterAddress(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto itr = route_table_.find(addr);
	if(route_table_.end() == itr)
	{
		LOG(GAMNET_ERR, "[Gamnet::Network::Router] can not find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}
	route_table_.erase(itr);
	LOG(GAMNET_INF, "[Gamnet::Network::Router] unregister uni-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ")");
	return true;
}

std::shared_ptr<Session> RouterCasterImpl_Uni::FindSession(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto itr = route_table_.find(addr);
	if(route_table_.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}

bool RouterCasterImpl_Multi::RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto& sessions = route_table_[addr.service_name];
	for(auto& session : sessions)
	{
		if(addr == session->router_address)
		{
			LOG(INF, "[Gamnet::Network::Router] duplicated multi-cast address(address:", addr.ToString(), ")");
			return false;
		}
	}
	sessions.push_back(router_session);
	LOG(INF, "[Gamnet::Network::Router] register multi-cast address success(address:", addr.ToString(), ")");
	return true;
}

int RouterCasterImpl_Multi::SendMsg(const Address& addr, const std::shared_ptr<Tcp::Packet>& packet)
{
	std::list<std::shared_ptr<Session>> sessions;
	{
		std::lock_guard<std::mutex> lo(lock_);
		auto itr = route_table_.find(addr.service_name);
		if(route_table_.end() == itr)
		{
			LOG(ERR, "[Gamnet::Network::Router] can not find 'MULTI_CAST' route info(service_name:", addr.service_name, ")");
			return 0;
		}

		for(auto& session : itr->second)
		{
			sessions.push_back(session);
		}
	}

	for(auto& session : sessions)
	{
		session->AsyncSend(packet);
	}
	return sessions.size();
}

bool RouterCasterImpl_Multi::UnregisterAddress(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto itr = route_table_.find(addr.service_name);
	if(route_table_.end() == itr)
	{
		LOG(GAMNET_ERR, "[Gamnet::Network::Router] Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	auto& sessions = itr->second;
	sessions.erase(std::remove_if(sessions.begin(), sessions.end(), [&addr](const std::shared_ptr<Session> session) -> bool {
		if(addr.id == session->router_address.id)
		{
			LOG(GAMNET_INF, "[Gamnet::Network::Router] unregister multi-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ")");
			return true;
		}
		return false;
	}));
	return true;
}

std::shared_ptr<Session> RouterCasterImpl_Multi::FindSession(const Address& addr)
{
	return nullptr;
}

bool RouterCasterImpl_Any::RegisterAddress(const Address& addr, const std::shared_ptr<Session>& router_session)
{
	std::lock_guard<std::mutex> lo(lock_);
	std::pair<int, SessionArray>& pairSessionArray = route_table_[addr.service_name];
	SessionArray& sessions = pairSessionArray.second;
	for(auto& session : sessions)
	{
		if(addr == session->router_address)
		{
			LOG(INF, "[Gamnet::Network::Router] duplicated any-cast address(address:", addr.ToString(), ")");
			return false;
		}
	}

	sessions.push_back(router_session);
	pairSessionArray.first = sessions.size()-1;
	LOG(INF, "[Gamnet::Network::Router] register any-cast address success(address:", addr.ToString(), ")");
	return true;
}

int RouterCasterImpl_Any::SendMsg(const Address& addr, const std::shared_ptr<Tcp::Packet>& packet)
{
	std::shared_ptr<Session> router_session = FindSession(addr);
	if(nullptr == router_session)
	{
		LOG(ERR, "[Gamnet::Network::Router] can not find 'ANY_CAST' route(route_address:", addr.ToString(), ")");
		return 0;
	}

	router_session->AsyncSend(packet);
	return 1;
}

bool RouterCasterImpl_Any::UnregisterAddress(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto itr = route_table_.find(addr.service_name);
	if(route_table_.end() == itr)
	{
		LOG(GAMNET_WRN, "[Gamnet::Network::Router] Can't find route info(service_name:", addr.service_name.c_str(), ", cast_type:ANY_CAST, server_id:", addr.id, ")");
		return false;
	}

	std::pair<int, SessionArray>& pairSessionArray = itr->second;
	SessionArray& arrSession = pairSessionArray.second;
	arrSession.erase(std::remove_if(arrSession.begin(), arrSession.end(), [&addr](const std::shared_ptr<Session> session) -> bool {
		if(addr.id == session->router_address.id)
		{
			LOG(GAMNET_INF, "[Gamnet::Network::Router] unregister any-cast address success (service_name:", addr.service_name, ", id:", addr.id, ")");
			return true;
		}
		return false;
	}));
	return true;
}

std::shared_ptr<Session> RouterCasterImpl_Any::FindSession(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto itr = route_table_.find(addr.service_name);
	if (route_table_.end() == itr)
	{
		LOG(ERR, "[Gamnet::Network::Router] can not find 'ANY_CAST' route info(service_name:", addr.service_name, ")");
		return nullptr;
	}

	std::pair<int, SessionArray>& pairSessionArray = itr->second;
	SessionArray& arrSession = pairSessionArray.second;

	if (0 >= arrSession.size())
	{
		LOG(ERR, "[Gamnet::Network::Router] can not find 'ANY_CAST' route info(service_name:", addr.service_name, ")");
		return nullptr;
	}
	return arrSession[pairSessionArray.first++ % arrSession.size()];
}

RouterCaster::RouterCaster()
{
	arrCasterImpl_[(int)Address::CAST_TYPE::UNI_CAST] = std::shared_ptr<RouterCasterImpl>(new RouterCasterImpl_Uni());
	arrCasterImpl_[(int)Address::CAST_TYPE::MULTI_CAST] = std::shared_ptr<RouterCasterImpl>(new RouterCasterImpl_Multi());
	arrCasterImpl_[(int)Address::CAST_TYPE::ANY_CAST] = std::shared_ptr<RouterCasterImpl>(new RouterCasterImpl_Any());
}

bool RouterCaster::RegisterAddress(const Address& addr, std::shared_ptr<Session> session)
{
	for(int i=0; i<(int)Address::CAST_TYPE::MAX; i++)
	{
		if(false == arrCasterImpl_[i]->RegisterAddress(addr, session))
		{
			return false;
		}
	}
	session->router_address = addr;
	return true;
}

int RouterCaster::SendMsg(const Address& addr, const std::shared_ptr<Buffer>& buffer)
{
	MsgRouter_SendMsg_Ntf ntf;
	ntf.msg_seq = addr.msg_seq;
	std::copy(buffer->ReadPtr(), buffer->ReadPtr() + buffer->Size(), std::back_inserter(ntf.buffer));

	std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
	if(nullptr == packet)
	{
		LOG(ERR, "fail to create packet instance(msg:MsgRouter_SendMsg_Ntf)");
		return 0;
	}

	if(false == packet->Write(ntf))
	{
		LOG(ERR, "fail to serialize message(MsgRouter_SendMsg_Ntf)");
		return 0;
	}

	if((int)Address::CAST_TYPE::MAX <= (unsigned int)addr.cast_type)
	{
		LOG(ERR, "cast_type:", (unsigned int)addr.cast_type, " is undefined cast_type");
		return 0;
	}

	return arrCasterImpl_[(unsigned int)addr.cast_type]->SendMsg(addr, packet);
}

bool RouterCaster::UnregisterAddress(const Address& addr)
{
	for(int i=0; i<(int)Address::CAST_TYPE::MAX; i++)
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
	return arrCasterImpl_[(unsigned int)addr.cast_type]->FindSession(addr);
}

}}}

