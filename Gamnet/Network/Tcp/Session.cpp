#include "Session.h"

#include "../../Library/MD5.h"
#include "../../Library/Random.h"
#include "../../Library/Time/Time.h"
#include "SessionManager.h"

namespace Gamnet { namespace Network { namespace Tcp {

std::string Session::GenerateSessionToken(uint32_t session_key)
{
	return md5(Format(session_key, time(nullptr), Random::Range(1, 99999999)));
}

Session::Session()
	: handover_safe(false)
	, recv_seq(0)
	, send_seq(0)
	, last_recv_time(0)
	, session_state(State::Invalid)
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

	protocol.Init();
	session_token = Session::GenerateSessionToken(session_key);

	recv_seq = 0;
	send_seq = 0;
	handover_safe = false;
	last_recv_time = time(nullptr);
	
	return true;
}

void Session::Clear()
{
	send_packets.clear();
	session_state = State::Invalid;
	Network::Session::Clear();
}

void Session::AsyncSend(const std::shared_ptr<Packet>& packet)
{
	if (true == packet->reliable)
	{
		auto self = shared_from_this();
		Dispatch([this, self, packet]() {
			if (Session::RELIABLE_PACKET_QUEUE_SIZE <= send_packets.size())
			{
				handover_safe = false;
				Close(ErrorCode::SendQueueOverflowError);
				return;
			}
			send_packets.push_back(packet); // keep send message util ack received
			Network::Session::AsyncSend(packet);
		});
		return;
	}

	Network::Session::AsyncSend(packet);
}

void Session::OnRead(const std::shared_ptr<Buffer>& buffer) 
{
	try {
		last_recv_time = time( nullptr );
		protocol.Parse(buffer);
		std::shared_ptr<Packet> packet = nullptr;
		while (packet = protocol.Pop())
		{
			auto self = shared_from_this();
			session_manager->OnReceive(self, packet);
		}
	}
	catch (const Exception& e)
	{
		handover_safe = false;
		throw e;
	}
}

void Session::Close(int reason)
{
	auto self = shared_from_this();
	Dispatch([this, self, reason]() {
		if (State::AfterAccept == session_state)
		{
			OnClose(reason);
			session_state = State::AfterCreate;
		}

		socket = nullptr;

		if(State::AfterCreate == session_state && false == handover_safe)
		{
			OnDestroy();
			session_state = State::Invalid;
			session_manager->Remove(self);
			protocol.Clear();
		}
	});
}

}}}