#include "Link.h"
#include "LinkManager.h"

namespace Gamnet { namespace Network {

static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

Link::Link(LinkManager* linkManager) : 
	socket(io_service_),
	link_manager(linkManager),
	link_key(0),
	read_buffer(nullptr),
	session(nullptr)	
{
}

Link::~Link()
{
}

bool Link::Init()
{
	link_key = ++LinkManager::link_key;
	read_buffer = Buffer::Create();
	send_buffers.clear();
	session = nullptr;

	if (nullptr == read_buffer)
	{
		LOG(GAMNET_ERR, "[link_key:", link_key, "] can not create read buffer");
		return false;
	}
	return true;
}

void Link::Connect(const char* host, int port, int timeout)
{
	if(nullptr == host)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPointerError), "[link_key:", link_key, "] invalid host name");
	}

	if(nullptr == session)
	{
		throw Exception(GAMNET_ERRNO(ErrorCode::NullPacketError), "[link_key:", link_key, "] link refers null session pointer");
	}

	boost::asio::ip::tcp::resolver resolver_(io_service_);
	boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({host, Format(port).c_str()}));

	auto self = shared_from_this();
	socket.async_connect(endpoint_, session->strand.wrap([self](const boost::system::error_code& ec){
		self->timer.Cancel();
		if(ec)
		{
			self->Close(ErrorCode::ConnectFailError);
			return;
		}
		else
		{
			if(nullptr == self->link_manager)
			{
				self->Close(ErrorCode::InvalidLinkManagerError);
				return;
			}
			if(nullptr == self->session)
			{
				self->Close(ErrorCode::InvalidSessionError);
				return;
			}
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
		timer.SetTimer(timeout*1000, session->strand.wrap([this]() {
			Log::Write(GAMNET_WRN, "[link_key:", this->link_key, ", session_key:", NULL == this->session ? 0 : this->session->session_key, "] connect timeout(ip:", this->remote_address.to_string(), ")");
			this->Close(ErrorCode::ConnectTimeoutError);
		}));
	}
}

void Link::Close(int reason)
{
	LOG(DEV, "[link_key:", link_key, "] reason:", reason);
	if(true == socket.is_open())
	{
		try {
			if (nullptr != link_manager)
			{
				link_manager->Close();
				link_manager->OnClose(shared_from_this(), reason);
			}
		}
		catch (const Exception& e)
		{
			LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		}

		socket.close();
	}
	AttachSession(nullptr);
}

void Link::AsyncRead()
{
	if(nullptr == session)
	{
		return;
	}
	auto self(shared_from_this());
	socket.async_read_some(boost::asio::buffer(read_buffer->WritePtr(), read_buffer->Available()),
		session->strand.wrap([self](boost::system::error_code ec, std::size_t readbytes) {
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

			try {
				self->OnRead(self->read_buffer);
			}
			catch(const Exception& e)
			{
				LOG(ERR, "link key:", self->link_key, ", session_key:", (nullptr != self->session ? self->session->session_key : 0), ", error_code:", e.error_code(), ", error_msg:", e.what());
				self->Close(e.error_code());
				return;
			}
			if(nullptr != self->read_buffer)
			{
				self->read_buffer->Clear();
				self->AsyncRead();
			}
		})
	);
}

void Link::AsyncSend(const char* buf, int len)
{
	std::shared_ptr<Buffer> buffer = Buffer::Create();
	if(NULL == buffer)
	{
		LOG(GAMNET_ERR, "can not create buffer(link_key:", link_key, ")");
		return;
	}
	buffer->Append(buf, len);
	AsyncSend(buffer);
}

void Link::AsyncSend(const std::shared_ptr<Buffer>& buffer)
{
	if (nullptr == session)
	{
		return;
	}
	auto self(shared_from_this());
	session->strand.wrap([self](const std::shared_ptr<Buffer>& buffer) {
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
	if (nullptr == session)
	{
		return;
	}
	if(false == send_buffers.empty())
	{
		auto self(shared_from_this());
		const std::shared_ptr<Buffer> buffer = send_buffers.front();
		boost::asio::async_write(socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()),
			session->strand.wrap([self](const boost::system::error_code& ec, std::size_t transferredBytes) {
				if (0 != ec)
				{
					self->Close(ErrorCode::Success); // no error, just closed socket
					return;
				}

				if (true == self->send_buffers.empty())
				{
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

void Link::AttachSession(const std::shared_ptr<Session> session)
{
	if(nullptr != this->session)
	{
		auto self(shared_from_this());
		this->session->strand.wrap([self](const std::shared_ptr<Session> self_session) {
			self_session->link = nullptr;
			self_session->remote_address = nullptr;
			self->session = nullptr;
		})(this->session);
	}

	if(nullptr != session)
	{
		auto self(shared_from_this());
		session->strand.wrap([self](const std::shared_ptr<Session> self_session) {
			self_session->link = self;
			self_session->remote_address = &(self->remote_address);
			self->session = self_session;
		})(session);
	}
}

}}


