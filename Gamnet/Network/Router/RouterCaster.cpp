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
	std::lock_guard<std::mutex> lo(lock_);
	if(false == route_table_.insert(std::make_pair(addr, router_session)).second)
	{
		LOG(INF, "[Gamnet::Router] duplicated uni-cast address(address:", addr.ToString(), ")");
		return false;
	}
	LOG(INF, "[Gamnet::Router] register uni-cast address success(address:", addr.ToString(), ")");
	return true;
}

bool RouterCasterImpl_Uni::SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Tcp::Session>& network_session, const Address& addr, const std::shared_ptr<Tcp::Packet>& packet)
{
	std::shared_ptr<Session> router_session = FindSession(addr);
	if(nullptr == router_session)
	{
		return false;
	}
	if(nullptr != network_session)
	{
		router_session->wait_session_manager.AddSession(msg_seq, network_session);
	}
	router_session->AsyncSend(packet);
	return true;
}

bool RouterCasterImpl_Uni::UnregisterAddress(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto itr = route_table_.find(addr);
	if(route_table_.end() == itr)
	{
		LOG(GAMNET_ERR, "[Gamnet::Router] can not find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}
	route_table_.erase(itr);
	LOG(GAMNET_INF, "[Gamnet::Router] unregister uni-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ")");
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
			LOG(INF, "[Gamnet::Router] duplicated multi-cast address(address:", addr.ToString(), ")");
			return false;
		}
	}
	sessions.push_back(router_session);
	LOG(INF, "[Gamnet::Router] register multi-cast address success(address:", addr.ToString(), ")");
	return true;
}

bool RouterCasterImpl_Multi::SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Tcp::Session>& network_session, const Address& addr, const std::shared_ptr<Tcp::Packet>& packet)
{
	std::list<std::shared_ptr<Session>> sessions;
	{
		std::lock_guard<std::mutex> lo(lock_);
		auto itr = route_table_.find(addr.service_name);
		if(route_table_.end() == itr)
		{
			LOG(GAMNET_WRN, "[Gamnet::Router] Can't find route info(service_name:", addr.service_name.c_str(), ", cast_type:MULTI_CAST, server_id:", addr.id, ")");
			return false;
		}

		for(auto& session : itr->second)
		{
			sessions.push_back(session);
		}
	}

	for(auto& session : sessions)
	{
		if(nullptr != network_session)
		{
			session->wait_session_manager.AddSession(msg_seq, network_session);
		}
		session->AsyncSend(packet);
	}
	return true;
}

bool RouterCasterImpl_Multi::UnregisterAddress(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto itr = route_table_.find(addr.service_name);
	if(route_table_.end() == itr)
	{
		LOG(GAMNET_ERR, "[Gamnet::Router] Cant find route info(service_name:", addr.service_name.c_str(), ")");
		return false;
	}

	auto& sessions = itr->second;
	sessions.erase(std::remove_if(sessions.begin(), sessions.end(), [&addr](const std::shared_ptr<Session> session) -> bool {
		if(addr.id == session->router_address.id)
		{
			LOG(GAMNET_INF, "[Gamnet::Router] unregister multi-cast address success (service_name:", addr.service_name.c_str(), ", id:", addr.id, ")");
			return true;
		}
		return false;
	}));
	return true;
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
			LOG(INF, "[Gamnet::Router] duplicated any-cast address(address:", addr.ToString(), ")");
			return false;
		}
	}

	sessions.push_back(router_session);
	pairSessionArray.first = sessions.size()-1;
	LOG(INF, "[Gamnet::Router] register any-cast address success(address:", addr.ToString(), ")");
	return true;
}

bool RouterCasterImpl_Any::SendMsg(uint64_t msg_seq, const std::shared_ptr<Network::Tcp::Session>& network_session, const Address& addr, const std::shared_ptr<Tcp::Packet>& packet)
{
	std::shared_ptr<Session> router_session = nullptr;

	{
		std::lock_guard<std::mutex> lo(lock_);
		auto itr = route_table_.find(addr.service_name);
		if(route_table_.end() == itr)
		{
			LOG(GAMNET_ERR, "[Gamnet::Router] Cant find route info(service_name:", addr.service_name.c_str(), ")");
			return false;
		}

		std::pair<int, SessionArray>& pairSessionArray = itr->second;
		SessionArray& arrSession = pairSessionArray.second;

		if(0 >= arrSession.size())
		{
			LOG(GAMNET_ERR, "[Gamnet::Router] Cant find Session");
			return false;
		}
		router_session = arrSession[pairSessionArray.first++ % arrSession.size()];
	}

	if(nullptr != network_session)
	{
		router_session->wait_session_manager.AddSession(msg_seq, network_session);
	}

	router_session->AsyncSend(packet);
	return true;
}

bool RouterCasterImpl_Any::UnregisterAddress(const Address& addr)
{
	std::lock_guard<std::mutex> lo(lock_);
	auto itr = route_table_.find(addr.service_name);
	if(route_table_.end() == itr)
	{
		LOG(GAMNET_WRN, "[Gamnet::Router] Can't find route info(service_name:", addr.service_name.c_str(), ", cast_type:ANY_CAST, server_id:", addr.id, ")");
		return false;
	}

	std::pair<int, SessionArray>& pairSessionArray = itr->second;
	SessionArray& arrSession = pairSessionArray.second;
	arrSession.erase(std::remove_if(arrSession.begin(), arrSession.end(), [&addr](const std::shared_ptr<Session> session) -> bool {
		if(addr.id == session->router_address.id)
		{
			LOG(GAMNET_INF, "[Gamnet::Router] unregister any-cast address success (service_name:", addr.service_name, ", id:", addr.id, ")");
			return true;
		}
		return false;
	}));
	return true;
}

RouterCaster::RouterCaster() 
{
	msg_seq = 0;
	arrCasterImpl_[(int)ROUTER_CAST_TYPE::UNI_CAST] = std::shared_ptr<RouterCasterImpl>(new RouterCasterImpl_Uni());
	arrCasterImpl_[(int)ROUTER_CAST_TYPE::MULTI_CAST] = std::shared_ptr<RouterCasterImpl>(new RouterCasterImpl_Multi());
	arrCasterImpl_[(int)ROUTER_CAST_TYPE::ANY_CAST] = std::shared_ptr<RouterCasterImpl>(new RouterCasterImpl_Any());
}

bool RouterCaster::RegisterAddress(const Address& addr, std::shared_ptr<Session> session)
{
	for(int i=0; i<(int)ROUTER_CAST_TYPE::MAX; i++)
	{
		if(false == arrCasterImpl_[i]->RegisterAddress(addr, session))
		{
			return false;
		}
	}
	session->router_address = addr;
	return true;
}

bool RouterCaster::SendMsg(const std::shared_ptr<Network::Tcp::Session>& network_session, const Address& addr, const char* buf, int len)
{
	MsgRouter_SendMsg_Ntf ntf;
	ntf.buffer.assign(buf, len);
	ntf.msg_seq = addr.msg_seq;
	if(nullptr != network_session)
	{
		ntf.msg_seq = ++msg_seq;
	}

	std::shared_ptr<Network::Tcp::Packet> packet = Network::Tcp::Packet::Create();
	if(nullptr == packet)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "fail to create packet instance(msg_id:", MsgRouter_SendMsg_Ntf::MSG_ID, ")");
	}

	packet->msg_seq = 0;
	packet->reliable = false;
	
	if(false == packet->Write(ntf))
	{
		throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, "fail to serialize message(msg_id:", MsgRouter_SendMsg_Ntf::MSG_ID, ")");
	}

	if((unsigned int)ROUTER_CAST_TYPE::MAX <= (unsigned int)addr.cast_type)
	{
		throw GAMNET_EXCEPTION(ErrorCode::RouterCastTypeErrror, "cast_type:", (unsigned int)addr.cast_type, " is undefined cast_type");
	}
	return arrCasterImpl_[(unsigned int)addr.cast_type]->SendMsg(ntf.msg_seq, network_session, addr, packet);
}

bool RouterCaster::UnregisterAddress(const Address& addr)
{
	for(int i=0; i<(int)ROUTER_CAST_TYPE::MAX; i++)
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
	std::shared_ptr<RouterCasterImpl_Uni> caster_impl = std::static_pointer_cast<RouterCasterImpl_Uni>(arrCasterImpl_[(int)ROUTER_CAST_TYPE::UNI_CAST]);
	return caster_impl->FindSession(addr);
}

}}}

