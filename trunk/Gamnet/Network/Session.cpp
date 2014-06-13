/*
 * Session.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: kukuta
 */
#include "Session.h"
#include "Listener.h"

namespace Gamnet { namespace Network {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>();

Session::Session()
	: socket_(io_service_),
	  strand_(io_service_),
	  sessionKey_(0),
	  listener_(NULL),
	  readBuffer_(NULL),
	  lastHeartBeatTime_(0)
{
}

Session::~Session()
{
}

void Session::OnError(int reason)
{
	OnClose(reason);
	listener_->OnClose(shared_from_this());
	socket_.close();
}

void Session::_read_start()
{
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(readBuffer_->WritePtr(), readBuffer_->Available()),
		strand_.wrap([this, self](boost::system::error_code ec, std::size_t readbytes) {
			if (0 != ec)
			{
				this->OnError(errno); // no error, just closed socket
				return;
			}
			lastHeartBeatTime_ = ::time(NULL);

			this->readBuffer_->writeCursor_ += readbytes;

			while(Packet::Header::PACKET_HEADER_SIZE <= (int)this->readBuffer_->Size())
			{
				const unsigned short bodyLength = this->readBuffer_->GetBodyLength();
				const unsigned short totalLength = this->readBuffer_->GetTotalLength();
				if(0 > bodyLength || totalLength >= Packet::Header::PACKET_MAX_LENGTH)
				{
					Log::Write(GAMNET_ERR, "buffer overflow(read size:", totalLength, ")");
					OnError(EOVERFLOW);
					return;
				}

				// 데이터가 부족한 경우
				if(totalLength > (unsigned short)this->readBuffer_->Size())
				{
					break;
				}

				std::shared_ptr<Packet> pBuffer = Packet::Create();
				if(NULL == pBuffer)
				{
					Log::Write(GAMNET_ERR, "Can't create more buffer(session_key:", this->sessionKey_, ")");
					OnError(EBUSY);
					return;
				}

				if(totalLength < (unsigned short)this->readBuffer_->Size())
				{
					pBuffer->Append(this->readBuffer_->ReadPtr() + totalLength, this->readBuffer_->Size() - totalLength);
				}

				listener_->OnRecvMsg(shared_from_this(), readBuffer_);
				readBuffer_ = pBuffer;
			}

			this->_read_start();
		})
	);
}

int	Session::Send(std::shared_ptr<Packet> packet)
{
	int transferredBytes = Send(packet->ReadPtr(), packet->Size());
	if(0 < transferredBytes)
	{
		packet->Remove(transferredBytes);
	}
	return transferredBytes;
}

int Session::Send(const char* buf, int len)
{
	int totalSentBytes = 0;
	while(len > totalSentBytes)
	{
		boost::system::error_code ec;
		int sentBytes = boost::asio::write(socket_, boost::asio::buffer(buf+totalSentBytes, len-totalSentBytes), ec);
		if(0 > sentBytes || 0 != ec)
		{
			Log::Write(GAMNET_ERR, "fail to send(session_key:", sessionKey_, ", errno:", errno, ", errstr:", strerror(errno), ")");
			return -1;
		}
		totalSentBytes += sentBytes;
	}
	return totalSentBytes;
}
}}


