#include "Session.h"
#include "Link.h"

namespace Gamnet { namespace Network { namespace Tcp {

Session::Session()
{
}

Session::~Session()
{
}

bool Session::Init()
{
	if(false == Network::Session::Init())
	{
		return false;
	}

	session_token = Session::GenerateSessionToken(session_key);

	recv_seq = 0;
	send_seq = 0;
	handover_safe = false;
	
	return true;
}

void Session::Clear()
{
	send_packets.clear();
	Network::Session::Clear();
}

void Session::AsyncSend(const std::shared_ptr<Packet> packet)
{
	if (true == packet->reliable)
	{
		auto self = std::static_pointer_cast<Session>(shared_from_this());
		strand.wrap([self](const std::shared_ptr<Packet> packet) {
			if (Session::RELIABLE_PACKET_QUEUE_SIZE <= self->send_packets.size())
			{
				self->handover_safe = false;
				if(nullptr != self->link)
				{
					self->link->Close(ErrorCode::SendQueueOverflowError);
				}
				return;
			}
			self->send_packets.push_back(packet); // keep send message util ack received
			self->link->AsyncSend(packet);
		})(packet);
		return;
	}

	Network::Session::AsyncSend(packet);
}

SessionManager::SessionManager()
{
}

SessionManager::~SessionManager()
{
}

bool SessionManager::Init()
{
	std::lock_guard<std::mutex> lo(_lock);
	_sessions.clear();
	return true;
}

bool SessionManager::Add(uint32_t key, const std::shared_ptr<Session>& session)
{
	std::lock_guard<std::mutex> lo(_lock);
	if (false == _sessions.insert(std::make_pair(key, session)).second)
	{
		LOG(GAMNET_ERR, "[link_key:", session->link->link_key, ", session_key:", key, "] duplicated session key");
		assert(!"duplicate session");
		return false;
	}

	return true;
}

void SessionManager::Remove(uint32_t key)
{
	std::lock_guard<std::mutex> lo(_lock);
	_sessions.erase(key);
}

std::shared_ptr<Session> SessionManager::Find(uint32_t key)
{
	std::lock_guard<std::mutex> lo(_lock);
	auto itr = _sessions.find(key);
	if (_sessions.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}

size_t SessionManager::Size()
{
	std::lock_guard<std::mutex> lo(_lock);
	return _sessions.size();
}

}}}