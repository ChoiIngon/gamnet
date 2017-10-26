#include "Link.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Link::Link()
	: socket(io_service_),
	strand(io_service_),
	link_key(0),
	msg_seq(0),
	expire_time(0),
	read_buffer(NULL),
	session(NULL),
	manager(NULL)
{
}

Link::~Link()
{
}

void Link::Connect(const char* host, int port, int timeout)
{
	if(NULL == host)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "[link_key:", link_key, "] invalid host name");
	}

	boost::asio::ip::tcp::resolver resolver_(io_service_);
	boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({host, Format(port).c_str()}));

	auto self = shared_from_this();
	socket.async_connect(endpoint_, strand.wrap([self](const boost::system::error_code& ec){
		self->timer.Cancel();
		if(ec)
		{
			self->OnError(ec.value());
		}
		else
		{
			if(NULL == self->session)
			{
				self->OnError(ErrorCode::NullPointerError);
				return;
			}
			try {
				boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
				self->socket.set_option(option);
				self->remote_address = self->socket.remote_endpoint().address();
				self->expire_time = 0;
				if(NULL != self->manager)
				{
					self->manager->OnConnect(self);
				}
				//LOG(DEV, "[link_key:", self->link_key, ", session_key:", self->session->session_key,"] connect success(remote_address:", self->remote_address.to_string(), ")");
				self->AsyncRead();
			}
			catch(const boost::system::system_error& e)
			{
				LOG(ERR, "[link_key:", self->link_key, "] connect fail(errno:", errno, ", errstr:", e.what(), ")");
			}
		}
	}));

	if(0 < timeout)
	{
		timer.SetTimer(timeout*1000, strand.wrap([self]() {
			Log::Write(GAMNET_WRN, "[link_key:", self->link_key, ", session_key:", NULL == self->session ? 0 : self->session->session_key, "] connect timeout(ip:", self->remote_address.to_string(), ")");
			self->OnError(ETIMEDOUT);
		}));
	}
}

void Link::OnError(int reason)
{
	if(false == socket.is_open())
	{
		return;
	}
	try {
		if(NULL != manager)
		{
			manager->OnClose(shared_from_this(), reason);
		}
		AttachManager(nullptr);
	}
	catch(const Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
	}

	socket.close();
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

			if(0 == readbytes)
			{
				self->OnError(errno);
				return;
			}

			self->expire_time = ::time(NULL);
			self->read_buffer->writeCursor_ += readbytes;
			if(nullptr == self->manager)
			{
				return;
			}
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
				LOG(ERR, "[link_key:", link_key, "] send fail(errno:", errno, ", errstr:", strerror(errno), ", ec:", ec, ")");
				return -1;
			}
			if(0 == sentBytes)
			{
				LOG(ERR, "[link_key:", link_key, "] send '0' byte(errno:", errno, ", errstr:", strerror(errno), ", ec:", ec, ")");
				return -1;
			}
			totalSentBytes += sentBytes;
		}
		catch(const boost::system::system_error& e)
		{
			LOG(ERR, "[link_key:", link_key, "] send exception(errno:", errno, ", errstr:", strerror(errno), ")");
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
		if(NULL != self->manager)
		{
			self->manager->Add(self->link_key, self);
		}
	})(manager);
}

void Link::AttachSession(const std::shared_ptr<Session> session)
{
	auto self(shared_from_this());
	strand.wrap([self](const std::shared_ptr<Session> session) {
		if (NULL != self->session) {
			//LOG(DEV, "[link_key:", self->link_key, ", session_key:", self->session->session_key, "] detach session");
			self->session->link = NULL;
			self->session = NULL;
		}
		self->session = session;
		if(NULL != self->session) {
			self->session->remote_address = &(self->remote_address);
			self->session->link = self;
			self->session->manager = self->manager;
			//LOG(DEV, "[link_key:", self->link_key, ", session_key:", self->session->session_key, "] attach session");
		}
	})(session);
}

}}


