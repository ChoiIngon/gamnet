#ifndef GAMNET_NETWORK_TCP_LINKMANAGER_H_
#define GAMNET_NETWORK_TCP_LINKMANAGER_H_

#include "Dispatcher.h"
#include "Packet.h"
#include "../LinkManager.h"

#include "MsgNetwork.h"

#include <memory>
#include <atomic>
#include <mutex>
#include <boost/bind.hpp>


namespace Gamnet { namespace Network { namespace Tcp {

template <class SESSION_T>
class LinkManager : public Network::LinkManager
{
public :
	Pool<SESSION_T, std::mutex, Session::Init> session_pool;
	SessionManager session_manager;

	LinkManager() : session_pool()	{}
	virtual ~LinkManager()	{}

	void Listen(int port, int max_session, int keepAliveTime)
	{
		session_pool.Capacity(max_session);
		session_manager.Init(keepAliveTime);
		Network::LinkManager::Listen(port, max_session, keepAliveTime);
	}

	virtual void OnAccept(const std::shared_ptr<Link>& link)
	{
		Network::LinkManager::OnAccept(link);
		const std::shared_ptr<SESSION_T> session = session_pool.Create();
		if(NULL == session)
		{
			link->OnError(EINVAL);
			return;
		}
		session->session_key = Network::Session::GenerateSessionKey(link->link_key);
		session_manager.Add(session->session_key, session);
		link->AttachSession(session);
		session->OnAccept();
	}

	virtual void OnClose(const std::shared_ptr<Link>& link, int reason)
	{
		const std::shared_ptr<Network::Session>& session = link->session;
		if(NULL != session)
		{
			session->OnClose(reason);
			link->AttachSession(std::shared_ptr<Network::Session>(NULL));
		}
		Network::LinkManager::OnClose(link, reason);
	}

	virtual void OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer)
	{
		const std::shared_ptr<SESSION_T>& session = std::static_pointer_cast<SESSION_T>(link->session);
		if(NULL == session)
		{
			LOG(GAMNET_ERR, "invalid session(link_key:", link->link_key, ")");
			link->OnError(EINVAL);
			return;
		}

		session->recv_packet->Append(buffer->ReadPtr(), buffer->Size());
		while(Packet::HEADER_SIZE <= (int)session->recv_packet->Size())
		{
			uint16_t totalLength = session->recv_packet->GetLength();
			if(Packet::HEADER_SIZE > totalLength )
			{
				LOG(GAMNET_ERR, "buffer underflow(read size:", totalLength, ")");
				link->OnError(EOVERFLOW);
				return;
			}

			if(totalLength >= session->recv_packet->Capacity())
			{
				LOG(GAMNET_ERR, "buffer overflow(read size:", totalLength, ")");
				link->OnError(EOVERFLOW);
				return;
			}

			// if received bytes are not enough
			if(totalLength > (uint16_t)session->recv_packet->Size())
			{
				break;
			}

			uint32_t msg_seq = session->recv_packet->GetSEQ();
			if(msg_seq > session->msg_seq)
			{
				Singleton<Dispatcher<SESSION_T>>::GetInstance().OnRecvMsg(session, session->recv_packet);
				session->msg_seq = msg_seq;
			}
#ifdef _DEBUG
			else
			{
				LOG(WRN, "same message seq(link_key:", link->link_key, ")");
			}
#endif
			session->recv_packet->Remove(totalLength);

			if(0 < session->recv_packet->Size())
			{
				std::shared_ptr<Packet> packet = Packet::Create();
				if (NULL == packet)
				{
					LOG(GAMNET_ERR, "Can't create more buffer(link_key:", link->link_key, ")");
					link->OnError(EOVERFLOW);
					return;
				}
				packet->Append(session->recv_packet->ReadPtr(), session->recv_packet->Size());
				session->recv_packet = packet;
			}
			else
			{
				session->recv_packet->Clear();
			}
		}
	}

	std::shared_ptr<SESSION_T> FindSession(const std::string& session_key) {
		return session_manager.Find(session_key);
	}
	
	template <class FUNC, class FACTORY>
	bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
	{
		return Singleton<Dispatcher<SESSION_T>>::GetInstance().RegisterHandler(msg_id, func, factory);
	}
};

}}}
#endif /* LISTENER_H_ */
