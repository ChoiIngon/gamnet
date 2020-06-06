#include "Session.h"
#include "SessionManager.h"
#include <list>
#include <future>
#include "../Log/Log.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Network {
	
static std::atomic<uint32_t> SESSION_KEY;
static boost::asio::io_service& io_service = Singleton<boost::asio::io_service>::GetInstance();

Session::Session() :
	session_key(0),
	session_manager(nullptr),
	socket(nullptr),
	strand(std::make_shared<boost::asio::strand>(io_service)),
	read_buffer(nullptr)
{
}

Session::~Session()
{
}

bool Session::Init()
{
	session_key = ++SESSION_KEY;
	read_buffer = Buffer::Create();
	return true;
}

void Session::Clear()
{
	session_key = 0;
}

void Session::AsyncSend(const char* data, size_t length)
{
	std::shared_ptr<Buffer> buffer = Buffer::Create();
	buffer->Append(data, length);
	AsyncSend(buffer);
}

void Session::AsyncSend(const std::shared_ptr<Buffer>& buffer)
{
	auto self = shared_from_this();
	strand->wrap([self] (const std::shared_ptr<Buffer>& buffer) {
		bool needFlush = self->send_buffers.empty();
		self->send_buffers.push_back(buffer);
		if (true == needFlush)
		{
			self->FlushSend();
		}
	})(buffer);
}

void Session::FlushSend()
{
	if (true == send_buffers.empty())
	{
		return;
	}

	if (nullptr == socket)
	{
		LOG(ERR, "invalid link[session_key:", session_key, "]");
		return;
	}
	
	auto self(shared_from_this());
	const std::shared_ptr<Buffer> buffer = send_buffers.front();
	boost::asio::async_write(*socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()), strand->wrap(std::bind(&Session::OnSendHandler, self, std::placeholders::_1, std::placeholders::_2)));
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

int Session::SyncSend(const std::shared_ptr<Buffer>& buffer)
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

void Session::Close(int reason)
{
	auto self(shared_from_this());
	strand->wrap([self](int reason) {
		if (nullptr == self->socket)
		{
			return;
		}
		self->OnClose(reason);
		self->socket = nullptr;
		self->session_manager->Remove(self);
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
			assert(nullptr != self->read_buffer);
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			self->Close(e.error_code());
			return;
		}
		self->read_buffer->Clear();
		if (nullptr == self->socket)
		{
			return;
		}
		self->AsyncRead();
	}));
}

}} /* namespace Gamnet */
