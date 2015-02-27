/*
 * Packet.h
 *
 *  Created on: 2012. 8. 16.
 *      Author: jjaehuny
 */

#ifndef GAMNET_NETWORK_PACKET_H_
#define GAMNET_NETWORK_PACKET_H_

#include <memory>
#include "../Library/Buffer.h"
#include "../Log/Log.h"

namespace Gamnet { namespace Network {

class Protocol
{
	std::shared_ptr<Buffer> buffer_;
	enum
	{
		LENGTH_PTR = 0,
		LENGTH_SIZE = sizeof(uint16_t),
		MSGID_PTR = LENGTH_PTR + LENGTH_SIZE,
		MSGID_SIZE = sizeof(uint32_t),
		HEADER_SIZE = MSGID_PTR + MSGID_SIZE
	};
public :
	static uint16_t HeaderSize()
	{
		return HEADER_SIZE;
	}
	Protocol(std::shared_ptr<Buffer> buffer) : buffer_(buffer) {}
	uint32_t MsgID() const
	{
		return  *((uint32_t*)(buffer_->buf_ + MSGID_PTR));
	}
	void MsgID(uint32_t msg_id)
	{
		(*(uint32_t*)(buffer_->buf_ + MSGID_PTR)) = msg_id;
	}
	uint16_t Length() const
	{
		return *((uint16_t*)(buffer_->buf_ + LENGTH_PTR));
	}
	void Length(uint16_t length)
	{
		(*(uint16_t*)(buffer_->buf_ + LENGTH_PTR)) = length + HEADER_SIZE;
	}
};

class Protocol_V2
{
	std::shared_ptr<Buffer> buffer_;
	enum
	{
		CHECKSUM_PTR = 0,
		CHECKSUM_SIZE = 1,
		LENGTH_PTR = CHECKSUM_PTR + CHECKSUM_SIZE,
		LENGTH_SIZE = sizeof(uint16_t),
		OPTION_PTR = LENGTH_PTR + LENGTH_SIZE,
		OPTION_SIZE = 1,
		MSGID_PTR = OPTION_PTR + OPTION_SIZE,
		MSGID_SIZE = sizeof(uint16_t),
		HEADER_SIZE = MSGID_PTR + MSGID_SIZE
	};
public :
	static uint16_t HeaderSize()
	{
		return HEADER_SIZE;
	}
	Protocol_V2(std::shared_ptr<Buffer> buffer) : buffer_(buffer) {}
	uint32_t MsgID() const
	{
		return  *((short*)(buffer_->buf_ + MSGID_PTR));
	}
	void MsgID(uint32_t msg_id)
	{
		(*(short*)(buffer_->buf_ + MSGID_PTR)) = msg_id;
	}
	uint16_t Length() const
	{
		return *((uint16_t*)(buffer_->buf_ + LENGTH_PTR)) + CHECKSUM_SIZE + LENGTH_SIZE;
	}
	void Length(uint16_t length)
	{
		(*(char*)(buffer_->buf_ + CHECKSUM_PTR)) = 0xff;
		(*(short*)(buffer_->buf_ + LENGTH_PTR)) = length - (CHECKSUM_SIZE + LENGTH_SIZE);
		(*(char*)(buffer_->buf_ + OPTION_PTR)) = 0x00;
	}
};

class Packet : public Buffer
{
public:
	enum
	{
		LENGTH_PTR = 0,
		LENGTH_SIZE = sizeof(uint16_t),
		MSGID_PTR = LENGTH_PTR + LENGTH_SIZE,
		MSGID_SIZE = sizeof(uint32_t),
		HEADER_SIZE = MSGID_PTR + MSGID_SIZE,
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

	uint32_t HeaderSize();
	uint16_t GetTotalLength() const;
	uint32_t GetID() const;

	/*
	 * byte stream -> Msg
	 */
	template <class MSG>
	bool Read(MSG& msg)
	{
		size_t bodyLength = GetTotalLength() - HEADER_SIZE;
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
			Log::Write(GAMNET_WRN, "packet max capacity over(msg_id:", msg_id, ", size:", total_length, ")");
			return false;
		}

		if((uint16_t)Available() < total_length)
		{
			Resize(total_length);
		}

		/*
		(*(char*)(buf_ + Header::PACKET_INDENTIFYER_PTR)) = 0xff;
		(*(short*)(buf_ + Header::PACKET_LENGTH_PTR)) = msg_size + 3;
		(*(char*)(buf_ + Header::PACKET_OPTION_PTR)) = 0x00;
		(*(short*)(buf_ + Header::PACKET_ID_PTR)) = msg_id;
		*/
		(*(uint16_t*)(buf_ + LENGTH_PTR)) = total_length;
		(*(uint32_t*)(buf_ + MSGID_PTR)) = msg_id;
		char* pBuf = buf_ + HEADER_SIZE;
		if(false == msg.Store(&pBuf))
		{
			return false;
		}
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

}}
#endif /* NETWORK_PACKET_H_ */
