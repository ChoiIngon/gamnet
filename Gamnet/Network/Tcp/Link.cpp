#include "Link.h"
#include "LinkManager.h"
#include "Session.h"

namespace Gamnet { namespace Network { namespace Tcp {

	Link::Link(Network::LinkManager* linkManager) : Network::Link(), link_manager(linkManager), session(nullptr)
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
			LOG(GAMNET_ERR, "[", link_manager->name, "/", link_key, "/0] can not create Packet instance");
			return false;
		}
	
		return true;
	}

	void Link::Clear()
	{
		recv_packet = nullptr;
		session = nullptr;
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
		if (false == link_manager->Add(shared_from_this()))
		{
			assert(!"duplicated link");
			throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "duplicated link");
		}
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
		if (false == link_manager->Add(shared_from_this()))
		{
			assert(!"duplicated link");
			throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "duplicated link");
		}
	}

	void Link::OnRead(const std::shared_ptr<Buffer>& buffer)
	{
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
			session->handover_safe = false;
			throw e;
		}
	}

	void Link::OnClose(int reason)
	{
		if (true == socket.is_open())
		{
			assert(false);
			return;
		}
		link_manager->OnClose(shared_from_this(), reason);
		link_manager->Remove(link_key);
	}
}}}
