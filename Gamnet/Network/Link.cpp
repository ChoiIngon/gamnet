#include "Link.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();
std::atomic<uint32_t> Link::link_key_generator;

Link::Link(LinkManager* linkManager) : 
	read_buffer(nullptr),
	socket(io_service_),
	strand(io_service_),
	link_key(0),
	session(nullptr),	
	link_manager(linkManager)
{
}

Link::~Link()
{
}

bool Link::Init()
{
	link_key = ++Link::link_key_generator;
	read_buffer = Buffer::Create();
	if (nullptr == read_buffer)
	{
		LOG(GAMNET_ERR, "[link_key:", link_key, "] can not create read buffer");
		return false;
	}
	remote_address = boost::asio::ip::address();
	send_buffers.clear();
	session = nullptr;
	return true;
}

void Link::Connect(const char* host, int port, int timeout)
{
	if(nullptr == host)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "[link_key:", link_key, "] invalid host name");
	}

	if(nullptr == link_manager)
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "[link_key:", link_key, "] invalid link manager");
	}

	boost::asio::ip::tcp::resolver resolver_(io_service_);
	boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({host, Format(port).c_str()}));

	auto self = shared_from_this();
	assert(self);
	socket.async_connect(endpoint_, strand.wrap([self](const boost::system::error_code& ec) {
		self->timer.Cancel();

		if(0 != ec)
		{
			self->Close(ErrorCode::ConnectFailError);
			return;
		}
		else
		{
			try {
				{
					boost::asio::socket_base::linger option(true, 0);
					self->socket.set_option(option);
				}
				{
					boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
					self->socket.set_option(option);
				}
				self->remote_address = self->socket.remote_endpoint().address();
				self->link_manager->OnConnect(self);
				self->AsyncRead();
				if (false == self->link_manager->Add(self))
				{
					assert(!"duplicated link");
					throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "[link_key:", self->link_key, "] duplicated link");
				}
			}
			catch(const boost::system::system_error& e)
			{
				LOG(ERR, "[link_key:", self->link_key, "] connect fail(errno:", errno, ", errstr:", e.what(), ")");
				self->Close(ErrorCode::UndefinedError);
			}
		}
	}));

	if(0 < timeout)
	{
		timer.AutoReset(false);
		timer.SetTimer(timeout*1000, strand.wrap([this]() {
			Log::Write(GAMNET_WRN, "[", link_manager->name, ", link_key:", link_key, "] connect timeout(ip:", remote_address.to_string(), ")");
			Close(ErrorCode::ConnectTimeoutError);
		}));
	}
}

void Link::AsyncRead()
{
	auto self(shared_from_this());
	socket.async_read_some(boost::asio::buffer(read_buffer->WritePtr(), read_buffer->Available()),
		strand.wrap([self](boost::system::error_code ec, std::size_t readbytes) {
			if (0 != ec)
			{
				self->Close(ErrorCode::Success); // no error, just closed socket
				return;
			}

			if(0 == readbytes)
			{
				self->Close(ErrorCode::Success);
				return;
			}

			self->read_buffer->writeCursor_ += readbytes;

			try 
			{
				self->OnRead(self->read_buffer);
				if(nullptr == self->read_buffer)
				{
					return;
				}
			}
			catch(const Exception& e)
			{
				LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), ", link key:", self->link_key, ", session_key:", (nullptr != self->session ? self->session->session_key : 0));
				self->Close(e.error_code());
				return;
			}
			self->read_buffer->Clear();
			self->AsyncRead();
		})
	);
}

void Link::AsyncSend(const char* buf, int len)
{
	std::shared_ptr<Buffer> buffer = Buffer::Create();
	if(nullptr == buffer)
	{
		LOG(GAMNET_ERR, "can not create buffer(link_key:", link_key, ")");
		return;
	}
	buffer->Append(buf, len);
	AsyncSend(buffer);
}

void Link::AsyncSend(const std::shared_ptr<Buffer>& buffer)
{
	auto self(shared_from_this());
	strand.wrap([self](const std::shared_ptr<Buffer>& buffer) {
		bool needFlush = self->send_buffers.empty();
		self->send_buffers.push_back(buffer);
		if(true == needFlush)
		{
			self->FlushSend();
		}
	})(buffer);
}

void Link::OnSend(const boost::system::error_code& ec, std::size_t transferredBytes)
{
	if (0 != ec)
	{
		LOG(ERR, "[link_key:", link_key, "] send error(error_code:", ec.value(), ", message:", ec.message(), ")");
		Close(ec.value());
		return;
	}

	if (true == send_buffers.empty())
	{
		return;
	}

	send_buffers.pop_front();
	FlushSend();
}

void Link::FlushSend()
{
	if(false == send_buffers.empty())
	{
		auto self(shared_from_this());
		const std::shared_ptr<Buffer> buffer = send_buffers.front();
		boost::asio::async_write(socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()), std::bind(&Link::OnSend, self, std::placeholders::_1, std::placeholders::_2));
	}
}

int	Link::SyncSend(const std::shared_ptr<Buffer>& buffer)
{
	int transferredBytes = SyncSend(buffer->ReadPtr(), buffer->Size());
	if(0 < transferredBytes)
	{
		buffer->Remove(transferredBytes);
	}
	return transferredBytes;
}

int Link::SyncSend(const char* buf, int len)
{
	int totalSentBytes = 0;
	while(len > totalSentBytes)
	{
		try {
			boost::system::error_code ec;
			int sentBytes = boost::asio::write(socket, boost::asio::buffer(buf+totalSentBytes, len-totalSentBytes), ec);
			if(0 > sentBytes || 0 != ec)
			{
				LOG(ERR, "[link_key:", link_key, "] send fail(errno:", errno, ", ec:", ec, ")");
				return -1;
			}
			if(0 == sentBytes)
			{
				LOG(ERR, "[link_key:", link_key, "] send '0' byte(errno:", errno, ", ec:", ec, ")");
				return -1;
			}
			totalSentBytes += sentBytes;
		}
		catch(const boost::system::system_error& e)
		{
			LOG(ERR, "[link_key:", link_key, "] send exception(errno:", errno, ", errstr:", e.what(), ")");
			return -1;
		}
	}
	return totalSentBytes;
}

void Link::Close(int reason)
{
	if (true == socket.is_open())
	{
		try
		{
			link_manager->OnClose(shared_from_this(), reason);
		}
		catch (const Exception& e)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		}

		socket.close();
	}
	link_manager->Remove(link_key);
	session = nullptr;
}

}}


