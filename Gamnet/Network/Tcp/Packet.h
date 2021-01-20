#ifndef GAMNET_NETWORK_TCP_PACKET_H_
#define GAMNET_NETWORK_TCP_PACKET_H_

#include <memory>
#include <cstring>
#include "../../Library/Buffer.h"
#include "../../Log/Log.h"
#include <list>

namespace Gamnet { namespace Network { namespace Tcp {

class Packet : public Buffer
{
public :
	static std::shared_ptr<Packet> Create();
	static size_t PoolSize();
	static size_t PoolAvailable();
	static size_t PoolCapacity();

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
		OFFSET_RELIABLE = 10,
		OFFSET_RESERVED = 11,
		HEADER_SIZE = 12,
		MAX_LENGTH = 16384
	};

	struct Init
	{
		Packet* operator() (Packet* packet);
	};
	struct Release
	{
		Packet* operator() (Packet* packet);
	};

public :
	Packet();
	virtual ~Packet();

	uint16_t length; // header + data length
	uint32_t msg_seq;
	uint32_t msg_id;
	bool reliable;
	
	/*
	 * byte stream -> Msg
	 */
	template <class MSG>
	bool Read(MSG& msg)
	{
		if(false == ReadHeader())
		{
			return false;
		}
		size_t bodyLength = length - HEADER_SIZE;
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
		length = (uint16_t)(HEADER_SIZE + msg.Size());
		msg_id = MSG::MSG_ID;
		
		if(false == WriteHeader())
		{
			return false;
		}
		
		char* pBuf = data + HEADER_SIZE;
		if(false == msg.Store(&pBuf))
		{
			return false;
		}
		this->write_index += length;
		return true;
	}
	bool Write(uint32_t msgID, const char* buf, size_t bytes);
	bool WriteHeader();
	bool ReadHeader();

public :
	class Protocol
	{
	public :
		Protocol();
		void Init();
		void Clear();
		void Parse(const std::shared_ptr<Buffer>& buffer);
		std::shared_ptr<Packet> Pop();
	private:
		std::shared_ptr<Packet> recv_packet;
		std::list<std::shared_ptr<Packet>> recv_packet_queue;
	};
};


}}}
#endif /* NETWORK_PACKET_H_ */
