/*
 * Session.cpp
 *
 *  Created on: Aug 8, 2014
 *      Author: kukuta
 */

#include "Session.h"
#include "Dispatcher.h"

namespace Gamnet {
namespace Http {

Session::Session() {
}

Session::~Session() {
}

void Session::AsyncRead()
{
	auto self(std::static_pointer_cast<Session>(shared_from_this()));
	socket_.async_read_some(boost::asio::buffer(readBuffer_->WritePtr(), readBuffer_->Available()),
		strand_.wrap([self](boost::system::error_code ec, std::size_t readbytes) {
			if (0 != ec)
			{
				self->OnError(errno); // no error, just closed socket
				return;
			}
			for(size_t readPtr=0; readPtr<readbytes; readPtr++)
			{
				if('\r' == *(self->readBuffer_->WritePtr() + readPtr) &&
				   '\n' == *(self->readBuffer_->WritePtr() + readPtr+1) )
				{
					const std::string request = self->readBuffer_->ReadPtr();
					const std::string init_line = request.substr(0, request.find("\n"));
					std::string uri = init_line.substr(init_line.find("/")+1);
					size_t uri_end = uri.find("?");
					size_t param_start = uri_end;
					size_t param_end = uri.find(" ");
					if(std::string::npos == uri_end)
					{
						uri_end = param_end;
					}
					std::string param;
					if(std::string::npos != param_start)
					{
						param = uri.substr(param_start+1, param_end-(param_start+1));
					}
					uri = uri.substr(0, uri_end);

					std::map<std::string, std::string> mapParam;

					while(0 < param.length())
					{
						size_t equal_pos = param.find("=");
						if(std::string::npos == equal_pos)
						{
							break;
						}
						size_t delim_pos = param.find("&");
						if(std::string::npos == delim_pos)
						{
							break;
						}

						std::string name = param.substr(0, equal_pos);
						std::string value = param.substr(equal_pos+1, delim_pos-(equal_pos+1));
						param = param.substr(delim_pos+1);
						mapParam.insert(std::make_pair(name, value));
					}

					Singleton<Dispatcher>::GetInstance().OnRecvMsg(self, uri, mapParam);
					self->Send("HTTP/1.0 200 OK\r\n", strlen("HTTP/1.0 200 OK\r\n"));
					self->OnError(errno); // no error, just closed socket
					return;
				}
			}
			self->readBuffer_->writeCursor_ += readbytes;


			/*
			while(Packet::HEADER_SIZE <= (int)self->readBuffer_->Size())
			{
				uint16_t totalLength = self->readBuffer_->GetTotalLength();
				uint16_t bodyLength = totalLength-Packet::HEADER_SIZE;
				if(0 > bodyLength || totalLength >= self->readBuffer_->Capacity())
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

				std::shared_ptr<Packet> pBuffer = Packet::Create();
				if(NULL == pBuffer)
				{
					Log::Write(GAMNET_ERR, "Can't create more buffer(session_key:", self->sessionKey_, ")");
					self->OnError(EBUSY);
					return;
				}

				if(totalLength < (unsigned short)self->readBuffer_->Size())
				{
					pBuffer->Append(self->readBuffer_->ReadPtr() + totalLength, self->readBuffer_->Size() - totalLength);
				}

				self->listener_->OnRecvMsg(self, self->readBuffer_);

			}
			*/
			self->AsyncRead();
		})
	);
}
} /* namespace Http */
} /* namespace Gamnet */
