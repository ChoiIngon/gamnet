/*
 * Session.cpp
 *
 *  Created on: Jul 11, 2013
 *      Author: kukuta
 */
#include "Session.h"
#include "Listener.h"

namespace Gamnet { namespace Network {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

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
	if(false == socket_.is_open())
	{
		return;
	}
	try {
		OnClose(reason);
	}
	catch(const Exception& e)
	{
		Log::Write(GAMNET_ERR, "exception at ", __FUNCTION__, "(what:", e.what(), ")");
	}

	socket_.close();
	listener_->OnClose(shared_from_this());
}

void Session::AsyncRead()
{
	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(readBuffer_->WritePtr(), readBuffer_->Available()),
		strand_.wrap([self](boost::system::error_code ec, std::size_t readbytes) {
			if (0 != ec)
			{
				self->OnError(errno); // no error, just closed socket
				return;
			}
			self->lastHeartBeatTime_ = ::time(NULL);
			self->readBuffer_->writeCursor_ += readbytes;

			while(Packet::HEADER_SIZE <= (int)self->readBuffer_->Size())
			{
				if(0 == self->sessionKey_)
				{
					return;
				}
				uint16_t totalLength = self->readBuffer_->GetTotalLength();
				if(Packet::HEADER_SIZE > totalLength )
				{
					Log::Write(GAMNET_ERR, "buffer underflow(read size:", totalLength, ")");
					self->OnError(EOVERFLOW);
					return;
				}

				if(totalLength >= self->readBuffer_->Capacity())
				{
					Log::Write(GAMNET_ERR, "buffer overflow(read size:", totalLength, ")");
					self->OnError(EOVERFLOW);
					return;
				}

				// 데이터가 부족한 경우
				if(totalLength > (unsigned short)self->readBuffer_->Size())
				{
					break;
				}

				self->listener_->OnRecvMsg(self, self->readBuffer_);
				self->readBuffer_->Remove(totalLength);

				std::shared_ptr<Packet> pBuffer = Packet::Create();
				if(NULL == pBuffer)
				{
					Log::Write(GAMNET_ERR, "Can't create more buffer(session_key:", self->sessionKey_, ")");
					self->OnError(EBUSY);
					return;
				}

				if(0 < (unsigned short)self->readBuffer_->Size())
				{
					pBuffer->Append(self->readBuffer_->ReadPtr(), self->readBuffer_->Size());
				}

				self->readBuffer_ = pBuffer;
			}

			self->AsyncRead();
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
		try {
			boost::system::error_code ec;
			int sentBytes = boost::asio::write(socket_, boost::asio::buffer(buf+totalSentBytes, len-totalSentBytes), ec);
			if(0 > sentBytes || 0 != ec)
			{
				Log::Write(GAMNET_ERR, "fail to send(session_key:", sessionKey_, ", errno:", errno, ", errstr:", strerror(errno), ")");
				return -1;
			}
			totalSentBytes += sentBytes;
		}
		catch(const boost::system::system_error& e)
		{
			Log::Write(GAMNET_ERR, "fail to send(session_key:", sessionKey_, ", errno:", errno, ", errstr:", e.what(), ")");
			return -1;
		}
	}
	return totalSentBytes;
}

}}


