#include "Link.h"
#include "LinkManager.h"
#include "Session.h"

namespace Gamnet { namespace Network { namespace Tcp {

	Link::Link(Network::LinkManager* linkManager) : Network::Link(), expire_time(0), link_manager(linkManager), session(nullptr)
	{
	}

	Link::~Link()
	{
	}

	bool Link::Init()
	{
		if(false == Network::Link::Init())
		{
			return false;
		}

		recv_packet = Packet::Create();
		if(nullptr == recv_packet)
		{
			LOG(GAMNET_ERR, "[", link_manager->name, "/", link_key, "/0] ", "ErrorCode::NullPacketError can not create Packet instance");
			return false;
		}
	
		expire_time = time(nullptr);

		return true;
	}

	void Link::Clear()
	{
		recv_packet = nullptr;
		session = nullptr;
		timer.Cancel();
		Network::Link::Clear();
	}

	void Link::OnAccept()
	{
		{
			boost::asio::socket_base::linger option(true, 0);
			socket.set_option(option);
		}
		{
			boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
			socket.set_option(option);
		}

		link_manager->OnAccept(shared_from_this());
	}

	void Link::OnConnect()
	{
		{
			boost::asio::socket_base::linger option(true, 0);
			socket.set_option(option);
		}
		{
			boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
			socket.set_option(option);
		}
		link_manager->OnConnect(shared_from_this());
	}

	void Link::OnRead(const std::shared_ptr<Buffer>& buffer)
	{
		expire_time = time(nullptr);
		try {
			while(0 < buffer->Size())
			{
				size_t readSize = std::min(buffer->Size(), recv_packet->Available());
				recv_packet->Append(buffer->ReadPtr(), readSize);
				buffer->Remove(readSize);

				while(Packet::HEADER_SIZE <= (int)recv_packet->Size())
				{
					recv_packet->ReadHeader();
					if (Packet::HEADER_SIZE > recv_packet->length)
					{
						throw GAMNET_EXCEPTION(ErrorCode::BufferUnderflowError, "buffer underflow(read size:", recv_packet->length, ")");
					}

					if (recv_packet->length >= (uint16_t)recv_packet->Capacity())
					{
						throw GAMNET_EXCEPTION(ErrorCode::BufferOverflowError, "buffer overflow(read size:", recv_packet->length, ")");
					}

					if(recv_packet->length > (uint16_t)recv_packet->Size())
					{
						break;
					}

					std::shared_ptr<Packet> packet = recv_packet;
					recv_packet = Packet::Create();
					if (nullptr == recv_packet)
					{
						throw GAMNET_EXCEPTION(ErrorCode::NullPacketError, "can not create Packet instance");
					}
					recv_packet->Append(packet->ReadPtr() + packet->length, packet->Size() - packet->length);
					link_manager->OnRecvMsg(shared_from_this(), packet);
				}
			}
		}
		catch(const Exception& e)
		{
			if(nullptr != session)
			{
				session->handover_safe = false;
			}
			throw e;
		}
	}

	void Link::OnClose(int reason)
	{
		link_manager->OnClose(shared_from_this(), reason);
	}

	void Link::SetKeepAlive(int expire_seconds)
	{
		if (0 < expire_seconds)
		{
			timer.AutoReset(false);
			auto self = std::static_pointer_cast<Link>(shared_from_this());
			timer.SetTimer(expire_seconds * 1000, strand.wrap([self, expire_seconds]() {
				int64_t now = time(nullptr);
				if (self->expire_time + expire_seconds >= now)
				{
					self->SetKeepAlive(expire_seconds);
					return;
				}
				
				if (nullptr != self->session)
				{
					self->session->handover_safe = false;
				}
				LOG(INF, "delete idle link(", self->link_manager->name, ", link_key:", self->link_key, ", session_key:", (nullptr != self->session ? self->session->session_key : 0), ")");
				self->Close(ErrorCode::IdleTimeoutError);
			}));
		}
	}
}}}
