#ifndef GAMNET_NETWORK_TCP_CASTGROUP_H_
#define GAMNET_NETWORK_TCP_CASTGROUP_H_

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include "Packet.h"
#include "Session.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Network { namespace Tcp {

class CastGroup {
	static std::atomic<uint32_t> seq_generator;
	std::mutex lock;
	std::map<uint32_t, std::shared_ptr<Session>> sessions;
public:
	struct Init {
		CastGroup* operator () (CastGroup* group) {
			group->group_seq = ++seq_generator;
			return group;
		}
	};
	uint32_t group_seq;

	CastGroup();
	virtual ~CastGroup();
	virtual void AddSession(const std::shared_ptr<Session>& session);
	virtual void DelSession(const std::shared_ptr<Session>& session);
	void Clear();
	size_t Size();

	template <class MSG>
	bool SendMsg(const MSG& msg, bool reliable = false)
	{
		if(false == reliable)
		{
			std::shared_ptr<Packet> packet = Packet::Create();
			if (nullptr == packet)
			{
				LOG(GAMNET_ERR, "fail to create packet instance(castgroup_seq:", group_seq, ", msg_id:", MSG::MSG_ID, ")");
				return false;
			}

			packet->reliable = false;
			packet->msg_seq = 0;

			if (false == packet->Write(msg))
			{
				LOG(GAMNET_ERR, "fail to serialize message(castgroup_seq:", group_seq, ", msg_id:", MSG::MSG_ID, ")");
				return false;
			}

			std::lock_guard<std::mutex> lo(lock);
			for (const auto itr : sessions)
			{
				std::shared_ptr<Session> session = itr.second;
				session->AsyncSend(packet);
			}
		}
		else 
		{
			std::lock_guard<std::mutex> lo(lock);
			for (auto itr : sessions)
			{
				std::shared_ptr<Session> session = itr.second;
				std::shared_ptr<Packet> packet = Packet::Create();
				if (nullptr == packet)
				{
					LOG(GAMNET_ERR, "fail to create packet instance(session_key:", session->session_key, ", msg_id:", MSG::MSG_ID, ")");
					return false;
				}

				packet->reliable = reliable;
				packet->msg_seq = ++session->send_seq;

				if (false == packet->Write(msg))
				{
					LOG(GAMNET_ERR, "fail to serialize message(session_key:", session->session_key, ", msg_id:", MSG::MSG_ID, ")");
					return false;
				}

				session->AsyncSend(packet);
			}
		}
		return true;
	}

	static std::shared_ptr<CastGroup> Create();
};
}}}
#endif
