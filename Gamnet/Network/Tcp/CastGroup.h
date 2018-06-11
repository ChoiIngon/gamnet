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
	size_t Size();

	template <class MSG>
	bool SendMsg(const MSG& msg)
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

		std::list<std::shared_ptr<Session>> session_list;
		{
			std::lock_guard<std::mutex> lo(lock);
			for (const auto itr : sessions)
			{
				session_list.push_back(itr.second);
			}
		}
		for (const auto session : session_list)
		{
			session->strand.wrap(std::bind(&Tcp::Session::AsyncSend, session, packet));
		}

		return true;
	}

	static std::shared_ptr<CastGroup> Create();
};
}}}
#endif
