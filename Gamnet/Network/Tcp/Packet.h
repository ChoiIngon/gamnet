/*
 * Packet.h
 *
 *  Created on: 2012. 8. 16.
 *      Author: jjaehuny
 */

#ifndef GAMNET_NETWORK_TCP_PACKET_H_
#define GAMNET_NETWORK_TCP_PACKET_H_

#include <memory>
#include <cstring>
#include "../../Library/Buffer.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Network { namespace Tcp {

class Packet : public Buffer
{
public:
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
			return packet;
		}
	};

public :
	Packet();
	virtual ~Packet();

	uint16_t GetLength() const;
	uint32_t GetSEQ() const;
	uint32_t GetID() const;

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
	bool Write(const MSG& msg)
	{
		Clear();
		uint32_t msg_id = MSG::MSG_ID;
		uint16_t msg_size = (uint16_t)msg.Size();
		uint16_t total_length = msg_size + HEADER_SIZE;
		if(Capacity() <= total_length)
		{
			LOG(GAMNET_WRN, "packet max capacity over(msg_id:", msg_id, ", size:", total_length, ")");
			return false;
		}

		if((uint16_t)Available() < total_length)
		{
			Resize(total_length);
		}

		(*(uint16_t*)(buf_ + OFFSET_LENGTH)) = total_length;
		(*(uint32_t*)(buf_ + OFFSET_MSGSEQ)) = 0;
		(*(uint32_t*)(buf_ + OFFSET_MSGID)) = msg_id;
		char* pBuf = buf_ + HEADER_SIZE;
		if(false == msg.Store(&pBuf))
		{
			return false;
		}
		this->writeCursor_ += total_length;
		return true;
	}
	bool Write(uint32_t msg_id, uint32_t msg_seq, const char* data, size_t length)
		{
			Clear();
			uint16_t total_length = length + HEADER_SIZE;
			if(Capacity() <= total_length)
			{
				LOG(GAMNET_WRN, "packet max capacity over(msg_id:", msg_id, ", size:", total_length, ")");
				return false;
			}

			if((uint16_t)Available() < total_length)
			{
				Resize(total_length);
			}

			(*(uint16_t*)(buf_ + OFFSET_LENGTH)) = total_length;
			(*(uint32_t*)(buf_ + OFFSET_MSGSEQ)) = msg_seq;
			(*(uint32_t*)(buf_ + OFFSET_MSGID)) = msg_id;
			std::memcpy(buf_ + HEADER_SIZE, data, length);
			this->writeCursor_ += total_length;
			return true;
		}

	static std::shared_ptr<Packet> Create();
	template <class MSG>
	static bool Load(MSG& msg, std::shared_ptr<Packet> packet)
	{
		if(false == packet->Read(msg))
		{
			return false;
		}
		return true;
	}
};

}}}
#endif /* NETWORK_PACKET_H_ */
