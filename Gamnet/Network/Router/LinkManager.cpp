#include "LinkManager.h"
#include "RouterHandler.h"
#include "../Tcp/Tcp.h"

namespace Gamnet { namespace Network { namespace Router {

std::function<void(const Address& addr)> LinkManager::onRouterAccept = [](const Address&) {};
std::function<void(const Address& addr)> LinkManager::onRouterClose = [](const Address&) {};
std::mutex LinkManager::lock;

LinkManager::LinkManager()
{
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
			LOG(DEV, "[Router] send heartbeat message");
			for(auto itr = _links.begin(); itr != _links.end();) {
				std::shared_ptr<Link> link = itr->second;
				link->AsyncSend(packet);
			}
		}
		this->heartbeat_timer.Resume();
	});

	if (false == _timer.SetTimer(5000, [this](){
				std::lock_guard<std::recursive_mutex> lo(_lock);
				time_t now_ = time(NULL);
				for(auto itr = _links.begin(); itr != _links.end();) {
					std::shared_ptr<Link> link = itr->second;
					if(0 != link->expire_time && link->expire_time + _keepalive_time < now_)
					{
						LOG(GAMNET_ERR, "[link_key:", link->link_key, "] idle link timeout(ip:", link->remote_address.to_string(), ")");
				        _links.erase(itr++);
				        link->strand.wrap(std::bind(&Link::OnError, link, ETIMEDOUT))();
				    }
				    else {
				        ++itr;
				    }
				}
				_timer.Resume();
			}))
			{
				throw Exception(GAMNET_ERRNO(ErrorCode::UndefinedError), "session time out timer init fail");
			}

	session_manager.Init(300);
	Network::LinkManager::Listen(port, 4096, 300);
}

void LinkManager::Connect(const char* host, int port, int timeout, const std::function<void(const Address& addr)>& onConnect, const std::function<void(const Address& addr)>& onClose)
{
	const std::shared_ptr<Network::Link> link = Create();
	if(NULL == link)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "cannot create link instance");
	}

	const std::shared_ptr<Session> session = session_pool.Create();
	if(NULL == session)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "cannot create session instance");
	}

	session->session_key = ++Network::SessionManager::session_key;
	session->recv_packet = Network::Tcp::Packet::Create();
	session->remote_address = &(link->remote_address);
	session->onRouterConnect = onConnect;
	session->onRouterClose = onClose;
	
	link->AttachSession(session);
	link->Connect(host, port, timeout);
}

void LinkManager::OnAccept(const std::shared_ptr<Network::Link>& link)
{
	Tcp::LinkManager<Session>::OnAccept(link);
	session_manager.Add(link->session->session_key, link->session);
	link->session->OnAccept();
}

void LinkManager::OnConnect(const std::shared_ptr<Network::Link>& link)
{	
	const std::shared_ptr<Session>& session = std::static_pointer_cast<Session>(link->session);
	if(NULL == link->session)
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
	const std::shared_ptr<Network::Session>& session = link->session;
	if (NULL != session)
	{
		session->OnClose(reason);
		session_manager.Remove(session->session_key);
		link->AttachSession(std::shared_ptr<Network::Session>(NULL));
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
