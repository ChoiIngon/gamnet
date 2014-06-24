/*
 * Router_MsgHandler.cpp
 *
 *  Created on: Jun 24, 2013
 *      Author: kukuta
 */

#include "RouterHandler.h"
#include "RouterListener.h"
#include "../Log/Log.h"
#include "../Network/Network.h"
#include "RouterCaster.h"
#include "Dispatcher.h"
namespace Gamnet { namespace Router {

void RouterHandler::Recv_SendMsg_Ntf(std::shared_ptr<Session> session, std::shared_ptr<Network::Packet> packet)
{
	MsgRouter_SendMsg_Ntf ntf;
	try {
		if(false == Gamnet::Network::Packet::Load(ntf, packet))
		{
			throw Gamnet::Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] ROUTER_ERROR_MESSAGE_FORMAT_ERROR");
		}

		if(NULL == Singleton<RouterCaster>::GetInstance().FindSession(session->addr))
		{
			throw Gamnet::Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] receive a message from unregistered address(ip:", session->remote_address_.to_string(), ", service_name:", session->addr.service_name, ")");
		}
	}
	catch(const Gamnet::Exception& e) {
		Log::Write(GAMNET_ERR, e.what(), "(error_code:", e.error_code(), ")");
		return;
	}

	Address addr = session->addr;
	addr.msg_seq = ntf.nKey;
	std::shared_ptr<Network::Packet> session_packet = Network::Packet::Create();
	session_packet->Clear();
	session_packet->Append(ntf.sBuf.c_str(), ntf.sBuf.length());
	Singleton<Dispatcher>::GetInstance().OnRecvMsg(addr, session_packet);
}
void RouterHandler::Recv_SetAddress_Req(std::shared_ptr<Session> session, std::shared_ptr<Network::Packet> packet)
{
	MsgRouter_SetAddress_Req req;
	MsgRouter_SetAddress_Ans ans;
	ans.nErrorCode = ROUTER_ERROR_SUCCESS;

	try {
		if(false == Gamnet::Network::Packet::Load(req, packet))
		{
			throw Gamnet::Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] ROUTER_ERROR_MESSAGE_FORMAT_ERROR");
		}
		Log::Write(GAMNET_INF, "[Router] recv SetAddress_Req (", session->remote_address_.to_string(), "->localhost, service_name:", req.tLocalAddr.service_name.c_str(), ")");
		session->addr = req.tLocalAddr;
		ans.tRemoteAddr = Singleton<RouterListener>::GetInstance().localAddr_;
	}
	catch(const Gamnet::Exception& e) {
		Log::Write(GAMNET_ERR, e.what(), "(error_code:", e.error_code(), ")");
		ans.nErrorCode = e.error_code();
	}

	Log::Write(GAMNET_INF, "[Router] send SetAddress_Ans (localhost->", session->remote_address_.to_string(), ", service_name:", req.tLocalAddr.service_name.c_str(), ")");
	SendMsg(session, ans);
}
void RouterHandler::Recv_SetAddress_Ans(std::shared_ptr<Session> session, std::shared_ptr<Network::Packet> packet)
{
	MsgRouter_SetAddress_Ans ans;
	try {
		if(false == Gamnet::Network::Packet::Load(ans, packet))
		{
			throw Gamnet::Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] ROUTER_ERROR_MESSAGE_FORMAT_ERROR");
		}

		if(ROUTER_ERROR_SUCCESS != ans.nErrorCode)
		{
			throw Gamnet::Exception(ans.nErrorCode, "[", __FILE__, ":", __func__, "@" , __LINE__, "] Recv_SetAddress_Ans fail");
		}

		Log::Write(GAMNET_INF, "[Router] recv SetAddress_Ans(", session->remote_address_.to_string(), "->localhost, service_name:", ans.tRemoteAddr.service_name, ")");
		Singleton<RouterCaster>::GetInstance().RegisterAddress(ans.tRemoteAddr, session);
	}
	catch(const Gamnet::Exception& e) {
		Log::Write(GAMNET_ERR, e.what(), "(error_code:", e.error_code(), ")");
	}

	if(Singleton<RouterListener>::GetInstance().localAddr_ != ans.tRemoteAddr)
	{
		Log::Write(GAMNET_INF, "[Router] send SetAddress_Ntf (localhost->", session->remote_address_.to_string(), ")");
		MsgRouter_SetAddress_Ntf ntf;
		SendMsg(session, ntf);
	}
}
void RouterHandler::Recv_SetAddress_Ntf(std::shared_ptr<Session> session, std::shared_ptr<Network::Packet> packet)
{
	MsgRouter_SetAddress_Ntf ntf;
	try {
		if(false == Gamnet::Network::Packet::Load(ntf, packet))
		{
			throw Gamnet::Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] ROUTER_ERROR_MESSAGE_FORMAT_ERROR");
		}
		Log::Write(GAMNET_INF, "[Router] recv SetAddress_Ntf (", session->remote_address_.to_string(), "->localhost)");
		Singleton<RouterCaster>::GetInstance().RegisterAddress(session->addr, session);
	}
	catch(const Gamnet::Exception& e) {
		Log::Write(GAMNET_ERR, e.what(), "(error_code:", e.error_code(), ")");
	}
}

}};
