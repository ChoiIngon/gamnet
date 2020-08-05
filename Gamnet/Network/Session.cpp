#include "Session.h"
#include "SessionManager.h"
#include <list>
#include <future>
#include "../Log/Log.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Network {
	
static std::atomic<uint32_t> SESSION_KEY;
static boost::asio::io_context& io_context = Singleton<boost::asio::io_context>::GetInstance();

Session::Session() :
	session_key(0),
	session_manager(nullptr),
	socket(nullptr),
	strand(std::make_shared<strand_t>(io_context.get_executor())),
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
	Dispatch([this, self, buffer] () {
		bool needFlush = send_buffers.empty();
		send_buffers.push_back(buffer);
		if (true == needFlush)
		{
			FlushSend();
		}
	});
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
	
	auto self = shared_from_this();
	const std::shared_ptr<Buffer> buffer = send_buffers.front();
	//boost::asio::async_write(*socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()), boost::asio::bind_executor(*strand, std::bind(&Session::OnSendHandler, self, std::placeholders::_1, std::placeholders::_2)));
	boost::asio::async_write(*socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()), Bind(std::bind(&Session::OnSendHandler, self, std::placeholders::_1, std::placeholders::_2)));
}

void Session::OnSendHandler(const boost::system::error_code& ec, std::size_t transferredBytes)
{
	if (0 != ec.value())
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
	Dispatch([self, this, &promise, buffer]() {
		if (nullptr == self->socket)
		{
			LOG(ERR, "invalid link[session_key:", self->session_key, "]");
			promise.set_value(-1);
			return;
		}

		int totalSentBytes = 0;
		while (0 < buffer->Size())
		{
			try {
				boost::system::error_code ec;
				int sentBytes = boost::asio::write(*(self->socket), boost::asio::buffer(buffer->ReadPtr(), buffer->Size()), ec);
				if (0 > sentBytes || 0 != ec.value())
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
				buffer->Remove(sentBytes);
			}
			catch (const boost::system::system_error& e)
			{
				LOG(ERR, "send exception(errno:", errno, ", errstr:", e.what(), ")");
				self->Close(errno);
				promise.set_value(-1);
				return;
			}
		}
		promise.set_value(totalSentBytes);
	});
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
	auto self = shared_from_this();
	Dispatch([this, self, reason]() {
		if (nullptr == socket)
		{
			return;
		}
		OnClose(reason);
		socket = nullptr;
		OnDestroy();
		session_manager->Remove(self);
	});
}

void Session::AsyncRead()
{
	assert(nullptr != socket);
	auto self = shared_from_this();
	socket->async_read_some(boost::asio::buffer(read_buffer->WritePtr(), read_buffer->Available()),
		Bind([this, self](boost::system::error_code ec, std::size_t readbytes) 
	{
		if (0 != ec.value())
		{
			Close(ErrorCode::Success);
			return;
		}

		if (0 == readbytes)
		{
			Close(ErrorCode::Success);
			return;
		}

		read_buffer->write_index += readbytes;
		try
		{
			OnRead(read_buffer);
			assert(nullptr != read_buffer);
		}
		catch (const Exception& e)
		{
			LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
			Close(e.error_code());
			return;
		}
		read_buffer->Clear();
		if (nullptr == socket)
		{
			return;
		}
		AsyncRead();
	}));
}

}} /* namespace Gamnet */
