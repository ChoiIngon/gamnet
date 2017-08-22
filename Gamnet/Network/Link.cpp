#include "Link.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Link::Link()
	: socket(io_service_),
	  strand(io_service_),
	  link_key(0),
	  msg_seq(0),
	  manager(NULL),
	  read_buffer(NULL),
	  heartbeat_time(0)
{
}

Link::~Link()
{
}

bool Link::Connect(const char* host, int port, int timeout)
{
	boost::asio::ip::tcp::resolver resolver_(io_service_);
	boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({host, Format(port).c_str()}));

	socket.async_connect(endpoint_, strand.wrap([&](const boost::system::error_code& ec){
		timer.Cancel();
		if(ec)
		{
			OnError(ec.value());
		}
		else
		{
			link_key = ++LinkManager::link_key;
			try {
				remote_address = socket.remote_endpoint().address();
				AsyncRead();
				if(NULL != manager)
				{
					manager->OnConnect(shared_from_this());
				}
			}
			catch(const boost::system::system_error& e)
			{
				LOG(GAMNET_ERR, "fail to accept(session_key:", link_key, ", errno:", errno, ", errstr:", e.what(), ")");
			}
		}
	}));

	if(0 < timeout)
	{
		timer.SetTimer(timeout*1000, strand.wrap([&]() {
			Log::Write(GAMNET_WRN, "connect timeout(ip:", endpoint_.address().to_string(), ")");
			OnError(ETIMEDOUT);
		}));
	}
	return true;
}

void Link::OnError(int reason)
{
	if(false == socket.is_open())
	{
		return;
	}
	try {
		OnClose(reason);
	}
	catch(const Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, "exception(error_code:", e.error_code(), ", message:", e.what(), ")");
	}

	socket.close();
}

void Link::OnClose(int reason)
{
	if(NULL != manager)
	{
		manager->OnClose(shared_from_this(), reason);
	}
}
void Link::AsyncRead()
{
	auto self(shared_from_this());
	socket.async_read_some(boost::asio::buffer(read_buffer->WritePtr(), read_buffer->Available()),
		strand.wrap([self](boost::system::error_code ec, std::size_t readbytes) {
			if (0 != ec)
			{
				self->OnError(errno); // no error, just closed socket
				return;
			}
			self->heartbeat_time = ::time(NULL);
			self->read_buffer->writeCursor_ += readbytes;
			self->manager->OnRecvMsg(self, self->read_buffer);
			self->read_buffer->Clear();
			self->AsyncRead();
		})
	);
}

void Link::AsyncSend(const char* buf, int len)
{
	std::shared_ptr<Buffer> buffer = Buffer::Create();
	if(NULL == buffer)
	{
		LOG(GAMNET_ERR, "Can't create more buffer(link_key:", link_key, ")");
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

void Link::FlushSend()
{
	if(false == send_buffers.empty())
	{
		auto self(shared_from_this());
		const std::shared_ptr<Buffer>& buffer = send_buffers.front();
		boost::asio::async_write(socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()),
			strand.wrap([self](const boost::system::error_code& ec, std::size_t transferredBytes) {
				if (0 != ec)
				{
					self->OnError(errno); // no error, just closed socket
					return;
				}

				self->send_buffers.pop_front();
				self->FlushSend();
			}
		));
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
				LOG(GAMNET_ERR, "fail to send(session_key:", link_key, ", errno:", errno, ", errstr:", strerror(errno), ", ec:", ec, ")");
				return -1;
			}
			if(0 == sentBytes)
			{
				LOG(GAMNET_WRN, "send zero byte(session_key:", link_key, ", errno:", errno, ", errstr:", strerror(errno), ", ec:", ec, ")");
				return -1;
			}
			totalSentBytes += sentBytes;
		}
		catch(const boost::system::system_error& e)
		{
			LOG(GAMNET_ERR, "fail to send(session_key:", link_key, ", errno:", errno, ", errstr:", e.what(), ")");
			return -1;
		}
	}
	return totalSentBytes;
}

void Link::AttachManager(LinkManager* manager)
{
	auto self(shared_from_this());
	strand.wrap([self](LinkManager* manager) {
		if(NULL != self->manager)
		{
			self->manager->Remove(self->link_key);
		}
		self->manager = manager;
	})(manager);
}

void Link::AttachSession(const std::shared_ptr<Session>& session)
{
	auto self(shared_from_this());
	strand.wrap([self](const std::shared_ptr<Session>& session) {
		self->session = session;
		if(NULL != session) {
			session->remote_address = &(self->remote_address);
			session->link = self;
			session->manager = self->manager;
		}
	})(session);
}

}}


