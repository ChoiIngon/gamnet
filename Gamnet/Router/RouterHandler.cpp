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

		if(NULL == Singleton<RouterCaster>().FindSession(session->addr))
		{
			throw Gamnet::Exception(0, "[", __FILE__, ":", __func__, "@" , __LINE__, "] receive a message from unregistered address(ip:", session->socket_.remote_endpoint().address().to_string(), ", service_name:", session->addr.service_name, ", id:", session->addr.id, ")");
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
	Singleton<Dispatcher>().OnRecvMsg(addr, session_packet);
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
		Log::Write(GAMNET_INF, "Router(id:", session->socket_.remote_endpoint().address().to_string(), "):recv SetAddress_Req(service_name:", req.tLocalAddr.service_name.c_str(), ", id:", req.tLocalAddr.id, ")");
		ans.tRemoteAddr = Singleton<RouterListener>().localAddr_;
	}
	catch(const Gamnet::Exception& e) {
		Log::Write(GAMNET_ERR, e.what(), "(error_code:", e.error_code(), ")");
		ans.nErrorCode = e.error_code();
	}
	session->addr = req.tLocalAddr;

	Log::Write(GAMNET_INF, "Router(id:", session->socket_.remote_endpoint().address().to_string(), "):send SetAddress_Ans(service_name:", req.tLocalAddr.service_name.c_str(), ", id:", req.tLocalAddr.id, ")");
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

		Log::Write(GAMNET_INF, "Router(id:", session->socket_.remote_endpoint().address().to_string(), "):recv SetAddress_Ans(service_name:", ans.tRemoteAddr.service_name, ", id:", ans.tRemoteAddr.id, ")");
		Singleton<RouterCaster>().RegisterAddress(ans.tRemoteAddr, session);
	}
	catch(const Gamnet::Exception& e) {
		Log::Write(GAMNET_ERR, e.what(), "(error_code:", e.error_code(), ")");
	}

	if(Singleton<RouterListener>().localAddr_ != ans.tRemoteAddr)
	{
		Log::Write(GAMNET_INF, "Router(id:", session->socket_.remote_endpoint().address().to_string(), "):send SetAddress_Ntf");
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
		Log::Write(GAMNET_INF, "Router(id:", session->socket_.remote_endpoint().address().to_string(), "):recv SetAddress_Ntf");
		Singleton<RouterCaster>().RegisterAddress(session->addr, session);
	}
	catch(const Gamnet::Exception& e) {
		Log::Write(GAMNET_ERR, e.what(), "(error_code:", e.error_code(), ")");
	}
}

}};
