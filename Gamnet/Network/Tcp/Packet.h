#ifndef GAMNET_NETWORK_TCP_PACKET_H_
#define GAMNET_NETWORK_TCP_PACKET_H_

#include <memory>
#include <cstring>
#include "../../Library/Buffer.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Network { namespace Tcp {

class Packet : public Buffer
{
public :
	static std::shared_ptr<Packet> Create();
	template <class MSG>
	static bool Load(MSG& msg, std::shared_ptr<Packet> packet)
	{
		if (false == packet->Read(msg))
		{
			return false;
		}
		return true;
	}
public :
	enum
	{
		OFFSET_LENGTH = 0,
		OFFSET_MSGSEQ = 2,
		OFFSET_MSGID = 6,
		HEADER_SIZE = 10,
		MAX_LENGTH = 16384
	};

	struct Init
	{
		Packet* operator() (Packet* packet)
		{
			packet->Clear();
			packet->reliable = false;
			return packet;
		}
	};

public :
	struct Header {
		uint32_t msg_id;
		uint32_t msg_seq;
		uint16_t length;
	};

	Packet();
	virtual ~Packet();

	bool reliable;

	uint32_t GetSEQ() const;
	uint32_t GetID() const;
	uint16_t GetLength() const;
	
	/*
	 * byte stream -> Msg
	 */
	template <class MSG>
	bool Read(MSG& msg)
	{
		size_t bodyLength = GetLength() - HEADER_SIZE;
		const char* pBuf = ReadPtr() + HEADER_SIZE;
		if(false == msg.Load(&pBuf, bodyLength))
		{
			return false;
		}
		return true;
	}

	template <class MSG>
	bool Write(uint32_t recv_seq, const MSG& msg)
	{
		Clear();
		uint16_t total_length = (uint16_t)(msg.Size() + HEADER_SIZE);
		uint32_t msg_id = MSG::MSG_ID;
		
		if(Capacity() <= total_length)
		{
			LOG(GAMNET_WRN, "packet max capacity over(msg_id:", msg_id, ", size:", total_length, ")");
			return false;
		}

		if((uint16_t)Available() < total_length)
		{
			Resize(total_length);
		}

		(*(uint16_t*)(data + OFFSET_LENGTH)) = total_length;
		(*(uint32_t*)(data + OFFSET_MSGSEQ)) = recv_seq;
		(*(uint32_t*)(data + OFFSET_MSGID)) = msg_id;
		char* pBuf = data + HEADER_SIZE;
		if(false == msg.Store(&pBuf))
		{
			return false;
		}
		this->writeCursor_ += total_length;
		return true;
	}
	bool Write(const Header& header, const char* buf)
	{
		Clear();
		if(HEADER_SIZE > header.length)
		{
			LOG(GAMNET_WRN, "invalid header length");
			return false;
		}
		if(Capacity() <= header.length)
		{
			LOG(GAMNET_WRN, "packet max capacity over(msg_id:", header.msg_id, ", size:", header.length, ")");
			return false;
		}

		if((uint16_t)Available() < header.length)
		{
			Resize(header.length);
		}

		(*(uint16_t*)(data + OFFSET_LENGTH)) = header.length;
		(*(uint32_t*)(data + OFFSET_MSGSEQ)) = header.msg_seq;
		(*(uint32_t*)(data + OFFSET_MSGID)) = header.msg_id;
		if(0 < header.length - HEADER_SIZE)
		{
			std::memcpy(data + HEADER_SIZE, buf, header.length - HEADER_SIZE);
		}
		this->writeCursor_ += header.length;
		return true;
	}
};

}}}
#endif /* NETWORK_PACKET_H_ */
