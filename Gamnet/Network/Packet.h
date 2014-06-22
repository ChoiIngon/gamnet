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
	struct Header
	{
		enum
		{
			PACKET_INDENTIFYER_PTR = 0,
			PACKET_INDENTIFYER_SIZE = 1,
			PACKET_LENGTH_PTR = PACKET_INDENTIFYER_PTR + PACKET_INDENTIFYER_SIZE,
			PACKET_LENGTH_SIZE = sizeof(unsigned short),
			PACKET_OPTION_PTR = PACKET_LENGTH_PTR + PACKET_LENGTH_SIZE,
			PACKET_OPTION_SIZE = 1,
			PACKET_ID_PTR = PACKET_OPTION_PTR + PACKET_OPTION_SIZE,
			PACKET_ID_SIZE = sizeof(unsigned short),
			PACKET_HEADER_SIZE = PACKET_ID_PTR + PACKET_ID_SIZE,
			PACKET_MAX_LENGTH = 8192
		};
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
	Packet() : Buffer(Header::PACKET_MAX_LENGTH)
	{
	}

	virtual ~Packet()
	{
	}

	static uint32_t HeaderSize()
	{
		return Header::PACKET_HEADER_SIZE;
	}

	template <class MSG>
	bool Write(const MSG& msg)
	{
		Clear();
		unsigned int msg_id = MSG::MSG_ID;
		unsigned short msg_size = msg.Size();
		unsigned short total_length = msg_size + Header::PACKET_HEADER_SIZE;
		if(Header::PACKET_MAX_LENGTH <= total_length)
		{
			Log::Write(GAMNET_WRN, "packet max capacity over(msg_id:", msg_id, ", size:", total_length, ")");
			return false;
		}

		if((unsigned short)Available() < total_length)
		{
			Resize(total_length);
		}

		/*
		memcpy(buf_ + Header::PACKET_LENGTH_PTR, (const char*)&total_length, Header::PACKET_LENGTH_SIZE);
		memcpy(buf_ + Header::PACKET_ID_PTR, (const char*)&msg_id, Header::PACKET_ID_SIZE);
		*/
		(*(char*)(buf_ + Header::PACKET_INDENTIFYER_PTR)) = 0xff;
		(*(short*)(buf_ + Header::PACKET_LENGTH_PTR)) = msg_size + 3;
		(*(char*)(buf_ + Header::PACKET_OPTION_PTR)) = 0x00;
		(*(short*)(buf_ + Header::PACKET_ID_PTR)) = msg_id;
		char* pBuf = buf_ + Header::PACKET_HEADER_SIZE;
		if(false == msg.Store(&pBuf))
		{
			return false;
		}
		this->writeCursor_ += total_length;
		return true;
	}

	unsigned short GetTotalLength() const
	{
		return *((unsigned short*)(buf_ + Header::PACKET_LENGTH_PTR)) + Header::PACKET_INDENTIFYER_SIZE + Header::PACKET_LENGTH_SIZE;
	}

	unsigned short GetBodyLength() const
	{
		return GetTotalLength() - Header::PACKET_HEADER_SIZE;
	}

	short GetID() const
	{
		return  *((short*)(buf_ + Header::PACKET_ID_PTR));
	}

	/*
	 * byte stream -> Msg
	 */
	template <class MSG>
	bool Read(MSG& msg)
	{
		size_t bodyLength = GetBodyLength();
		Remove(Header::PACKET_HEADER_SIZE);
		const char* pBuf = ReadPtr();

		if(false == msg.Load(&pBuf, bodyLength))
		{
			return false;
		}
		Remove(bodyLength);
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
