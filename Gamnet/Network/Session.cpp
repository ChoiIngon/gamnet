#include "Session.h"
#include "Link.h"
#include "LinkManager.h"
#include "../Library/MD5.h"
#include "../Library/Random.h"
#include "../Library/Timer.h"
#include <list>
#include <future>

namespace Gamnet { namespace Network {
	

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

static std::atomic<uint32_t> SESSION_KEY;

std::string Session::GenerateSessionToken(uint32_t session_key)
{
	return md5(Format(session_key, time(nullptr), Random::Range(1, 99999999)));
}


Session::Session() :
	strand(io_service_),
	session_key(0),
	session_token(""),
	link(nullptr)
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
	link = nullptr;
	handler_container.Init();
}

void Session::AsyncSend(const std::shared_ptr<Buffer> buffer)
{
	auto self = shared_from_this();
	strand.wrap([self](const std::shared_ptr<Buffer> buffer) {
		if(nullptr == self->socket)
		{
			LOG(ERR, "invalid link[session_key:", self->session_key, "]");
			return;
		}

		bool needFlush = send_buffers.empty();
		send_buffers.push_back(buffer);
		if (true == needFlush)
		{
			FlushSend();
		}
	})(buffer);
}

void Session::AsyncSend(const char* data, size_t length)
{
	std::shared_ptr<Buffer> buffer = Buffer::Create();
	buffer->Append(data, length);
	AsyncSend(buffer);
}

int Session::SyncSend(const std::shared_ptr<Buffer> buffer)
{
	std::promise<int> sentBytes;
	//int sentBytes = -1;
	auto self = shared_from_this();
	strand.wrap([self, &sentBytes](const std::shared_ptr<Buffer> buffer) {
		if (nullptr == self->link)
		{
			LOG(ERR, "invalid link[session_key:", self->session_key, "]");
			sentBytes.set_value(-1);
			//sentBytes = -1;
			return;
		}
		sentBytes.set_value(self->link->SyncSend(buffer));
		//sentBytes = self->link->SyncSend(buffer);
	})(buffer);
	return sentBytes.get_future().get();
	//return sentBytes;
}

int Session::SyncSend(const char* data, int length)
{
	std::shared_ptr<Buffer> buffer = Buffer::Create();
	buffer->Append(data, length);
	return SyncSend(buffer);
}

const boost::asio::ip::address& Session::GetRemoteAddress() const
{
	assert(nullptr != link);
	return link->remote_address;
}

void Session::OnAcceptHandler()
{
}

void Session::FlushSend()
{
	if (false == send_buffers.empty())
	{
		auto self(shared_from_this());
		const std::shared_ptr<Buffer> buffer = send_buffers.front();
		boost::asio::async_write(socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()), strand.wrap(std::bind(&Link::OnSendHandler, self, std::placeholders::_1, std::placeholders::_2)));
	}
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
	strand.wrap([self](int reason) {
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


}} /* namespace Gamnet */
