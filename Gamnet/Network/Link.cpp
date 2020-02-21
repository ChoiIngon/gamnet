#include "Link.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();
static std::atomic<uint32_t> LINK_KEY;

Link::Link() :
	read_buffer(nullptr),
	socket(io_service_),
	strand(io_service_),
	link_key(0)
{
}

Link::~Link()
{
}

bool Link::Init()
{
	link_key = ++LINK_KEY;
	remote_address = boost::asio::ip::address();
	read_buffer = Buffer::Create();
	if (nullptr == read_buffer)
	{
		LOG(GAMNET_ERR, "can not create read buffer");
		return false;
	}
	
	return true;
}

void Link::Clear()
{
	read_buffer = nullptr;
	send_buffers.clear();
}

void Link::Connect(const char* host, int port, int timeout)
{
	if(nullptr == host)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidAddressError, "host is null");
	}

	if(0 == strlen(host))
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "host is empty");
	}

	boost::asio::ip::tcp::resolver resolver_(io_service_);
	boost::asio::ip::tcp::resolver::query query_(host, "");
	for (auto itr = resolver_.resolve(query_); itr != boost::asio::ip::tcp::resolver::iterator(); ++itr)
	{
		boost::asio::ip::tcp::endpoint end = *itr;
		remote_address = end.address();
		break;
	}
	
	boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({ remote_address.to_v4().to_string(), Format(port).c_str() }));
	
	auto self = shared_from_this();
	assert(self);

	if (0 < timeout)
	{
		timer.AutoReset(false);
		timer.SetTimer(timeout * 1000, strand.wrap(std::bind(&Link::Close, self, ErrorCode::ConnectTimeoutError)));
	}
	socket.async_connect(endpoint_, std::bind(&Link::OnConnectHandler, self, std::placeholders::_1, endpoint_));
}

void Link::OnAcceptHandler()
{
	remote_address = socket.remote_endpoint().address();
	OnAccept();
	AsyncRead();
}

void Link::OnConnectHandler(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint)
{
	try {
		timer.Cancel();
		if(false == socket.is_open())
		{
			throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[link_key:", link_key, "] connect timeout(dest:", remote_address.to_v4().to_string(), ", message:", ec.message(), ", errno:", ec, ")");
		}
		else if (0 != ec)
		{
			throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[link_key:", link_key, "] connect fail(dest:", remote_address.to_v4().to_string(), ", message:", ec.message(), ", errno:", ec, ")");
		} 
				
		OnConnect();
		AsyncRead();
		return;
	}
	catch(const Exception& e)
	{
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), ", error_code:", e.error_code());
	}
	catch (const boost::system::system_error& e)
	{
		LOG(ERR, "connect fail(dest:", remote_address.to_v4().to_string(), ", errno:", e.code().value(), ", errstr:", e.what(), ")");
	}
	Close(ErrorCode::ConnectFailError);
}

void Link::AsyncRead()
{
	auto self(shared_from_this());
	socket.async_read_some(boost::asio::buffer(read_buffer->WritePtr(), read_buffer->Available()),
		strand.wrap([self](boost::system::error_code ec, std::size_t readbytes) {
			if (0 != ec)
			{
				self->Close(ErrorCode::Success);
				return;
			}

			if(0 == readbytes)
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
			catch(const Exception& e)
			{
				LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
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
	strand.wrap([self](std::shared_ptr<Buffer> buffer) {
		bool needFlush = self->send_buffers.empty();
		self->send_buffers.push_back(buffer);
		if(true == needFlush)
		{
			self->FlushSend();
		}
	})(buffer);
}

void Link::FlushSend()
{
	if(false == send_buffers.empty())
	{
		auto self(shared_from_this());
		const std::shared_ptr<Buffer> buffer = send_buffers.front();
		boost::asio::async_write(socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()), strand.wrap(std::bind(&Link::OnSendHandler, self, std::placeholders::_1, std::placeholders::_2)));
	}
}

void Link::OnSendHandler(const boost::system::error_code& ec, std::size_t transferredBytes)
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
	OnClose(reason);
	if(true == socket.is_open())
	{
		socket.close();
	}
}

}}
