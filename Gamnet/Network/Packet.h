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

	bool Write(unsigned int msg_id, const char* buf, size_t len)
	{
		Clear();
		unsigned short total_length = len + Header::PACKET_HEADER_SIZE;
		if(Header::PACKET_MAX_LENGTH <= total_length)
		{
			Log::Write(GAMNET_WRN, "packet max capacity over(msg_id:", msg_id, ", size:", len, ")");
			return false;
		}

		if((unsigned short)Available() < total_length)
		{
			Resize(total_length);
		}

		Append((const char*)&total_length, Header::PACKET_LENGTH_SIZE);
		Append((const char*)&msg_id, Header::PACKET_ID_SIZE);
		Append(buf, len);
		return true;
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

	unsigned int GetID() const
	{
		return  *((unsigned int*)(buf_ + Header::PACKET_ID_PTR));
	}

	/*
	 * byte stream -> Msg
	 */
	template <class MSG>
	bool Read(MSG& msg)
	{
		Remove(Header::PACKET_HEADER_SIZE);
		const char* pBuf = ReadPtr();
		size_t bodyLength = GetBodyLength();
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
