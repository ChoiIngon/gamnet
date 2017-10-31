#include "LinkManager.h"
#include "RouterHandler.h"
#include "../Tcp/Tcp.h"

namespace Gamnet { namespace Network { namespace Router {

std::function<void(const Address& addr)> LinkManager::onRouterAccept = [](const Address&) {};
std::function<void(const Address& addr)> LinkManager::onRouterClose = [](const Address&) {};
std::mutex LinkManager::lock;

LinkManager::LinkManager()
{
	_name = "RouterLinkManager";
}

LinkManager::~LinkManager() {
}

void LinkManager::Listen(const char* service_name, int port, const std::function<void(const Address& addr)>& onAccept, const std::function<void(const Address& addr)>& onClose)
{
	LinkManager::onRouterAccept = onAccept;
	LinkManager::onRouterClose = onClose;

	RegisterHandler(MsgRouter_SetAddress_Req::MSG_ID, &RouterHandler::Recv_SetAddress_Req, new Network::HandlerStatic<RouterHandler>());
	RegisterHandler(MsgRouter_SetAddress_Ans::MSG_ID, &RouterHandler::Recv_SetAddress_Ans, new Network::HandlerStatic<RouterHandler>());
	RegisterHandler(MsgRouter_SetAddress_Ntf::MSG_ID, &RouterHandler::Recv_SetAddress_Ntf, new Network::HandlerStatic<RouterHandler>());
	RegisterHandler(MsgRouter_SendMsg_Ntf::MSG_ID, &RouterHandler::Recv_SendMsg_Ntf, new Network::HandlerStatic<RouterHandler>());
	RegisterHandler(MsgRouter_HeartBeat_Ntf::MSG_ID, &RouterHandler::Recv_HeartBeat_Ntf, new Network::HandlerStatic<RouterHandler>());
	local_address.service_name = service_name;
	local_address.cast_type = ROUTER_CAST_TYPE::UNI_CAST;
	local_address.id = Network::Tcp::GetLocalAddress().to_v4().to_ulong();
	if(0 == local_address.id)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::InvalidAddressError), "unique router id is not set");
	}

	heartbeat_timer.SetTimer(60000, [this] () {
		std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
		if(NULL != packet) {
			MsgRouter_HeartBeat_Ntf ntf;
			packet->Write(0, ntf);
			std::lock_guard<std::recursive_mutex> lo(_lock);
			//LOG(DEV, "[Router] send heartbeat message(link count:", _links.size(), ")");
			for(auto itr : _links) {
				std::shared_ptr<Link> link = itr.second;
				link->AsyncSend(packet);
			}
		}
		this->heartbeat_timer.Resume();
	});

	session_manager.Init(0);
	Network::LinkManager::Listen(port, 4096, 0);
}

void LinkManager::Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose)
{
	const std::shared_ptr<Network::Link> link = Create();
	if(NULL == link)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "cannot create link instance");
	}
	link->AttachManager(this);

	const std::shared_ptr<Session> session = session_pool.Create();
	if(NULL == session)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "cannot create session instance");
	}

	session->session_key = ++Network::SessionManager::session_key;
	session->recv_packet = Network::Tcp::Packet::Create();
	if(nullptr == session->recv_packet)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::CreateInstanceFailError), "cannot create packet");
	}
	session->onRouterConnect = onConnect;
	session->onRouterClose = onClose;

	link->AttachSession(session);
	link->Connect(host, port, timeout);
}

void LinkManager::OnAccept(const std::shared_ptr<Network::Link>& link)
{
	Tcp::LinkManager<Session>::OnAccept(link);
	const std::shared_ptr<Network::Session> session = link->session;
	if(NULL == session)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "[link_key:", link->link_key,"] invalid session");
	}
	session->OnAccept();
}

void LinkManager::OnConnect(const std::shared_ptr<Network::Link>& link)
{	
	const std::shared_ptr<Session>& session = std::static_pointer_cast<Session>(link->session);
	if(nullptr == link->session)
	{
		link->OnError(ErrorCode::InvalidSessionError);
		return;
	}
	session->session_token = Network::Session::GenerateSessionToken(session->session_key);
	session_manager.Add(session->session_key, session);
	session->OnConnect();
}

void LinkManager::OnClose(const std::shared_ptr<Network::Link>& link, int reason)
{
	const std::shared_ptr<Network::Session> session = link->session;
	if (nullptr != session)
	{
		session->OnClose(reason);
		session_manager.Remove(session->session_key);
		link->AttachSession(nullptr);
	}
	Network::LinkManager::OnClose(link, reason);
}

void LinkManager::OnRecvMsg(const std::shared_ptr<Link>& link, const std::shared_ptr<Buffer>& buffer)
{
	const std::shared_ptr<Session> session = std::static_pointer_cast<Session>(link->session);
	if (NULL == session)
	{
		LOG(GAMNET_ERR, "[link_key:", link->link_key, "] invalid session");
		link->OnError(EINVAL);
		return;
	}

	session->recv_packet->Append(buffer->ReadPtr(), buffer->Size());
	while (Tcp::Packet::HEADER_SIZE <= (int)session->recv_packet->Size())
	{
		uint16_t totalLength = session->recv_packet->GetLength();
		if (Tcp::Packet::HEADER_SIZE > totalLength)
		{
			LOG(GAMNET_ERR, "[link_key:", link->link_key, "] buffer underflow(read size:", totalLength, ")");
			link->OnError(EOVERFLOW);
			return;
		}

		if (totalLength >= session->recv_packet->Capacity())
		{
			LOG(GAMNET_ERR, "[link_key:", link->link_key, "] buffer overflow(read size:", totalLength, ")");
			link->OnError(EOVERFLOW);
			return;
		}

		// if received bytes are not enough
		if (totalLength > (uint16_t)session->recv_packet->Size())
		{
			break;
		}

		Singleton<Tcp::Dispatcher<Session>>::GetInstance().OnRecvMsg(session, session->recv_packet);
		
		session->recv_packet->Remove(totalLength);

		if (0 < session->recv_packet->Size())
		{
			std::shared_ptr<Tcp::Packet> packet = Tcp::Packet::Create();
			if (NULL == packet)
			{
				LOG(GAMNET_ERR, "[link_key:", link->link_key, "] Can't create more buffer");
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
}}}
