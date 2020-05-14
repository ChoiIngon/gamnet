#include "Session.h"
#include "../Library/MD5.h"
#include "../Library/Random.h"
#include "../Library/Timer.h"
#include <list>
#include <future>

namespace Gamnet { namespace Network {
	
static std::atomic<uint32_t> SESSION_KEY;

std::string Session::GenerateSessionToken(uint32_t session_key)
{
	return md5(Format(session_key, time(nullptr), Random::Range(1, 99999999)));
}


Session::Session() :
	socket(nullptr),
	strand(nullptr),
	read_buffer(nullptr),
	session_key(0),
	session_token(""),
	session_manager(nullptr)
{
}

Session::~Session()
{
}

bool Session::Init()
{
	session_key = ++SESSION_KEY;
	return true;
}

void Session::Clear()
{
	session_key = 0;
	session_token = "";
	handler_container.Init();
}

void Session::AsyncSend(const std::shared_ptr<Buffer> buffer)
{
	auto self = shared_from_this();
	strand->wrap([self](const std::shared_ptr<Buffer> buffer) {
		if(nullptr == self->socket)
		{
			LOG(ERR, "invalid link[session_key:", self->session_key, "]");
			return;
		}

		bool needFlush = self->send_buffers.empty();
		self->send_buffers.push_back(buffer);
		if (true == needFlush)
		{
			self->FlushSend();
		}
	})(buffer);
}

void Session::AsyncSend(const char* data, size_t length)
{
	std::shared_ptr<Buffer> buffer = Buffer::Create();
	buffer->Append(data, length);
	AsyncSend(buffer);
}

void Session::FlushSend()
{
	if (false == send_buffers.empty())
	{
		auto self(shared_from_this());
		const std::shared_ptr<Buffer> buffer = send_buffers.front();
		boost::asio::async_write(*socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()), strand->wrap(std::bind(&Session::OnSendHandler, self, std::placeholders::_1, std::placeholders::_2)));
	}
}

int Session::SyncSend(const std::shared_ptr<Buffer> buffer)
{
	std::promise<int> promise;
	auto self = shared_from_this();
	strand->wrap([self, &promise](const std::shared_ptr<Buffer> buffer) {
		if (nullptr == self->socket)
		{
			LOG(ERR, "invalid link[session_key:", self->session_key, "]");
			promise.set_value(-1);
			return;
		}

		int totalSentBytes = 0;
		while (buffer->Size() > totalSentBytes)
		{
			try {
				boost::system::error_code ec;
				int sentBytes = boost::asio::write(*(self->socket), boost::asio::buffer(buffer->ReadPtr() + totalSentBytes, buffer->Size() - totalSentBytes), ec);
				if (0 > sentBytes || 0 != ec)
				{
					LOG(ERR, "send fail(errno:", errno, ", ec:", ec, ")");
					promise.set_value(-1);
					return;
				}
				if (0 == sentBytes)
				{
					LOG(ERR, "send '0' byte(errno:", errno, ", ec:", ec, ")");
					promise.set_value(-1);
					return;
				}
				totalSentBytes += sentBytes;
			}
			catch (const boost::system::system_error& e)
			{
				LOG(ERR, "send exception(errno:", errno, ", errstr:", e.what(), ")");
				promise.set_value(-1);
				return;
			}
		}
		promise.set_value(totalSentBytes);
	})(buffer);
	return promise.get_future().get();
}

int Session::SyncSend(const char* data, int length)
{
	std::shared_ptr<Buffer> buffer = Buffer::Create();
	buffer->Append(data, length);
	return SyncSend(buffer);
}

void Session::OnAcceptHandler()
{
}

void Session::OnSendHandler(const boost::system::error_code& ec, std::size_t transferredBytes)
{
	if (0 != ec)
	{
		//Close(); // no error, just closed socket
		return;
	}

	if (true == send_buffers.empty())
	{
		return;
	}
	send_buffers.pop_front();
	FlushSend();
}

void Session::Close(int reason)
{
	auto self(shared_from_this());
	strand->wrap([self](int reason) {
		if (nullptr == self->socket)
		{
			return;
		}

		self->OnClose(reason);

		if (true == self->socket->is_open())
		{
			self->socket->close();
			self->socket = nullptr;
		}
	})(reason);
	
}

void Session::AsyncRead()
{
	assert(nullptr != socket);
	auto self(shared_from_this());
	socket->async_read_some(boost::asio::buffer(read_buffer->WritePtr(), read_buffer->Available()),
		strand->wrap([self](boost::system::error_code ec, std::size_t readbytes) 
	{
		if (0 != ec)
		{
			self->Close(ErrorCode::Success);
			return;
		}

		if (0 == readbytes)
		{
			self->Close(ErrorCode::Success);
			return;
		}

		self->read_buffer->write_index += readbytes;
		try
		{
			self->OnRead(self->read_buffer);
			if (nullptr == self->socket)
			{
				return;
			}
			assert(nullptr != self->read_buffer);
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			self->Close(e.error_code());
			return;
		}
		self->read_buffer->Clear();
		self->AsyncRead();
	}));
}

}} /* namespace Gamnet */
