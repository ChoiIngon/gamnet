/*
 * Router.h
 *
 *  Created on: Jun 8, 2014
 *      Author: kukuta
 */

#ifndef ROUTER_H_
#define ROUTER_H_

#include "../Network/Network.h"
#include "RouterListener.h"
#include "RouterCaster.h"
#include "MsgRouter.h"
#include "Dispatcher.h"

namespace Gamnet { namespace Router {

typedef Address Address;

void Listen(const char* service_name, int port=20001, int max_session = 4096);
void Connect(const char* host, int port=20001, int timeout=5);
template <class FUNC, class FACTORY>
bool RegisterHandler(unsigned int msg_id, FUNC func, FACTORY factory)
{
	return Singleton<Dispatcher>().RegisterHandler(msg_id, func, factory);
}

struct NullType { enum { MSG_ID = 0 }; };
template <class REQ, class ANS=NullType>
bool SendMsg(std::shared_ptr<Network::Session> session, const Address& addr, const REQ& msg)
{
	uint64_t msg_seq = 0;
	if(0 != ANS::MSG_ID && ROUTER_CAST_MULTI != addr.cast_type)
	{
		msg_seq = ++RouterListener::msgSeq_;
	}
	MsgRouter_SendMsg_Ntf ntf;
	{
		std::shared_ptr<Network::Packet> packet = Network::Packet::Create();
		if(NULL == packet)
		{
			return false;
		}
		if(false == packet->Write(msg))
		{
			return false;
		}
		ntf.nKey = msg_seq;
		ntf.sBuf.assign(packet->ReadPtr(), packet->Size());
	}

	std::shared_ptr<Network::Packet> packet = Network::Packet::Create();
	if(NULL == packet)
	{
		return false;
	}
	if(false == packet->Write(ntf))
	{
		return false;
	}
	if(false == Singleton<RouterCaster>().SendMsg(addr, packet->ReadPtr(), packet->Size()))
	{
		return false;
	}
	if(0 != msg_seq)
	{
		std::shared_ptr<Session> router_session = Singleton<RouterCaster>().FindSession(addr);
		if(NULL != router_session)
		{
			router_session->watingSessionManager_.AddSession(msg_seq, session);
		}
	}
	return true;
}

template <class REQ, class ANS>
bool SendMsg(std::shared_ptr<Network::Session>, const Address& addr, const REQ& msg, int ans=ANS::MSG_ID)
{
	return true;
}

}}



#endif /* ROUTER_H_ */
