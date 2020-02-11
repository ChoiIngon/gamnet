#include "Link.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();
static std::atomic<uint32_t> LINK_KEY;

Link::Link(LinkManager* linkManager) :
	read_buffer(nullptr),
	socket(io_service_),
	strand(io_service_),
	link_key(0),
	expire_time(0),
	session(nullptr),	
	link_manager(linkManager)
{
}

Link::~Link()
{
}

bool Link::Init()
{
	link_key = ++LINK_KEY;
	remote_address = boost::asio::ip::address();
	expire_time = time(nullptr);
	read_buffer = Buffer::Create();
	if (nullptr == read_buffer)
	{
		LOG(GAMNET_ERR, "[", link_manager->name, "/", link_key, "/0] can not create read buffer");
		return false;
	}
	return true;
}

void Link::Clear()
{
	session = nullptr;
	read_buffer = nullptr;
	send_buffers.clear();
}

void Link::Connect(const char* host, int port, int timeout)
{
	if (nullptr == link_manager)
	{
		throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "[link_key:", link_key, "] invalid link_manager");
	}

	if(nullptr == host)
	{
		throw GAMNET_EXCEPTION(ErrorCode::InvalidAddressError, "[", link_manager->name, "/", link_key, "/0] host is null");
	}

	if(0 == strlen(host))
	{
		throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "[", link_manager->name, "/", link_key, "/0] host is empty");
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
		timer.SetTimer(timeout * 1000, strand.wrap(std::bind(&Link::Close, self/*, ErrorCode::ConnectTimeoutError*/)));
	}
	socket.async_connect(endpoint_, strand.wrap(std::bind(&Link::OnConnect, self, std::placeholders::_1, endpoint_)));
}

void Link::OnAccept()
{
	boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
	socket.set_option(option);
	remote_address = socket.remote_endpoint().address();
	if (false == link_manager->Add(shared_from_this()))
	{
		assert(!"duplicated link");
		throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "duplicated link");
	}
	link_manager->OnAccept(shared_from_this());
	//strand.wrap(std::bind(&Link::AsyncRead, shared_from_this()))();
	AsyncRead();
}

void Link::OnConnect(const boost::system::error_code& ec, const boost::asio::ip::tcp::endpoint& endpoint)
{
	try {
		timer.Cancel();
		if(false == socket.is_open())
		{
			throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[", link_manager->name, "/", link_key, "0] connect timeout(dest:", remote_address.to_v4().to_string(), ", errno:", ec, ", link_manager:", link_manager->name, ")");
		}
		else if (0 != ec)
		{
			throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError, "[", link_manager->name, "/", link_key, "0] connect fail(dest:", remote_address.to_v4().to_string(), ", errno:", ec, ", link_manager:", link_manager->name, ")");
		} 
		
		{
			boost::asio::socket_base::linger option(true, 0);
			socket.set_option(option);
		}
		{
			boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
			socket.set_option(option);
		}
		//remote_address = socket.remote_endpoint().address();
		link_manager->OnConnect(shared_from_this());
		if (false == link_manager->Add(shared_from_this()))
		{
			assert(!"duplicated link");
			throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "duplicated link");
		}
		//strand.wrap(std::bind(&Link::AsyncRead, shared_from_this()))();
		AsyncRead();
		return;
	}
	catch(const Exception& e)
	{
		LOG(Log::Logger::LOG_LEVEL_ERR, e.what(), ", error_code:", e.error_code());
	}
	catch (const boost::system::system_error& e)
	{
		LOG(ERR, "[", link_manager->name, "/", link_key, "/0] connect fail(dest:", remote_address.to_v4().to_string(), ", errno:", e.code().value(), ", errstr:", e.what(), ")");
	}
	Close(/*ErrorCode::ConnectFailError*/);
}

void Link::AsyncRead()
{
	auto self(shared_from_this());
	socket.async_read_some(boost::asio::buffer(read_buffer->WritePtr(), read_buffer->Available()),
		[self](boost::system::error_code ec, std::size_t readbytes) {
			if (0 != ec)
			{
				self->socket.close(ec);
				self->OnClose(ErrorCode::Success); // no error, just closed socket
				return;
			}

			if(0 == readbytes)
			{
				self->socket.close();
				self->OnClose(ErrorCode::Success);
				return;
			}

			self->expire_time = time(nullptr);
			self->read_buffer->write_index += readbytes;

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
				LOG(Log::Logger::LOG_LEVEL_ERR, e.what());
				self->socket.close();
				//return;
			}
			self->read_buffer->Clear();
			//self->strand.wrap(std::bind(&Link::AsyncRead, self))();
			self->AsyncRead();
		}
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
		boost::asio::async_write(socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()), strand.wrap(std::bind(&Link::OnSend, self, std::placeholders::_1, std::placeholders::_2)));
	}
}

void Link::OnSend(const boost::system::error_code& ec, std::size_t transferredBytes)
{
	if (0 != ec)
	{
		Close(/*ErrorCode::Success*/); // no error, just closed socket
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

void Link::Close(/*int reason*/)
{
	if (false == socket.is_open())
	{
		return;
	}
	socket.close();
}

void Link::OnClose(int reason)
{
	if (true == socket.is_open())
	{
		assert(false);
		return;
	}

	LOG(INF, "[", link_manager->name, "/", link_key, "/", (nullptr == session ? 0 : session->session_key), "] Link::Close(reason:", reason, ")");
	link_manager->OnClose(shared_from_this(), reason);
	link_manager->Remove(link_key);
	session = nullptr;
}

void Link::AttachSession(const std::shared_ptr<Session>& session)
{
	this->session = session;
}

}}
