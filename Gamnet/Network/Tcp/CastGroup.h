#ifndef GAMNET_NETWORK_TCP_CASTGROUP_H_
#define GAMNET_NETWORK_TCP_CASTGROUP_H_

#include <map>
#include <memory>
#include "Packet.h"
#include "Session.h"
#include "../../Library/Atomic.h"

namespace Gamnet { namespace Network { namespace Tcp {

class CastGroup 
{
public :
	friend class LockGuard;
	struct Init
	{
		CastGroup* operator () (CastGroup* group);
	};
	
	class LockGuard
	{
		std::shared_ptr<CastGroup> ptr;
	public:
		LockGuard(const std::shared_ptr<CastGroup>& obj);
		~LockGuard();

		std::shared_ptr<CastGroup> operator -> ();
	};
public :
	CastGroup();
	virtual ~CastGroup();

private :
	std::mutex lock;

public :
	uint32_t group_seq;
	std::map<uint32_t, std::shared_ptr<Session>> sessions;

	size_t Insert(const std::shared_ptr<Session>& session);
	size_t Remove(const std::shared_ptr<Session>& session);
	void Clear();
	size_t Size();

	template <class MSG>
	bool SendMsg(const MSG& msg, bool reliable = true)
	{
		if (false == reliable)
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

			for (const auto itr : sessions)
			{
				std::shared_ptr<Session> session = itr.second;
				session->AsyncSend(packet);
			}
		}
		else
		{
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

public:
	static std::shared_ptr<CastGroup> Create();
};

}}}
#endif
