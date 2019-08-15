#include "Subscriber.h"

namespace Gamnet { namespace Database { namespace Redis {

	static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

	Subscriber::Subscriber() :
		socket(io_service_),
		strand(io_service_),
		deadline_timer(io_service_)
	{
		handlers.insert(std::make_pair("subscribe", std::bind(&Subscriber::OnRecv_SubscribeAns, this, std::placeholders::_1)));
		handlers.insert(std::make_pair("message", std::bind(&Subscriber::OnRecv_PublishReq, this, std::placeholders::_1)));
	}

	Subscriber::~Subscriber()
	{
	}

	bool Subscriber::Connect(const char* host, int port, int timeout)
	{
		if (nullptr == host)
		{
			throw GAMNET_EXCEPTION(ErrorCode::NullPointerError, "invalid host name");
		}

		deadline_timer.expires_at(boost::posix_time::pos_infin);
		deadline_timer.async_wait([this](boost::system::error_code) {
			if (this->deadline_timer.expires_at() <= boost::asio::deadline_timer::traits_type::now())
			{
				// The deadline has passed. The socket is closed so that any outstanding
				// asynchronous operations are cancelled. This allows the blocked
				// connect(), read_line() or write_line() functions to return.
				this->Close(ErrorCode::ConnectTimeoutError);

				// There is no longer an active deadline. The expiry is set to positive
				// infinity so that the actor takes no action until a new deadline is set.
				this->deadline_timer.expires_at(boost::posix_time::pos_infin);
			}
		});

		boost::asio::ip::tcp::resolver resolver_(io_service_);
		boost::asio::ip::tcp::resolver::query query_(host, "");
		for (auto itr = resolver_.resolve(query_); itr != boost::asio::ip::tcp::resolver::iterator(); ++itr)
		{
			boost::asio::ip::tcp::endpoint end = *itr;
			remote_address = end.address();
			break;
		}
		LOG(INF, "[Redis] connect...(host:", remote_address.to_v4().to_string(), ", port:", port, ")");
		boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({ remote_address.to_v4().to_string(), Format(port).c_str() }));

		boost::system::error_code ec;
		socket.connect(endpoint_, ec);
		if (0 != ec)
		{
			LOG(GAMNET_ERR, "[Redis] connect fail(host:", host, ", port:", port, ")");
			return false;
		}

		AsyncRead();
		return true;
	}

	void Subscriber::Close(int reason)
	{
		if(false == socket.is_open())
		{
			return;
		}
		LOG(INF, "[Redis] connect close(reason:", reason, ", dest:", remote_address.to_v4().to_string(), ")");
		socket.close();
	}

	void Subscriber::AsyncSend(const std::string& query)
	{
		std::shared_ptr<Buffer> buffer = Buffer::Create();
		buffer->Append(query.c_str(), query.length());
		buffer->Append("\r\n", 2);
		auto self = shared_from_this();
		boost::asio::async_write(socket, boost::asio::buffer(buffer->ReadPtr(), buffer->Size()),
			strand.wrap([self, buffer](const boost::system::error_code& ec, std::size_t transferredBytes) {
				if(0 != ec)
				{
					self->Close(ec.value());
					return;
				}
			}
		));
	}

	void Subscriber::AsyncRead()
	{
		if (false == socket.is_open())
		{
			return;
		}

		auto self = std::static_pointer_cast<Subscriber>(shared_from_this());
		boost::asio::async_read_until(socket, streambuf, "\r\n", strand.wrap([self](const boost::system::error_code& ec, std::size_t readBytes) 
		{
			const char* data = boost::asio::buffer_cast<const char*>(self->streambuf.data());
			size_t size = self->streambuf.size();
			std::string input = std::string(data, size);
			std::shared_ptr<ResultSetImpl> impl = std::make_shared<ResultSetImpl>();
			if(false == impl->Parse(input))
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError);
			}

			if(1 > impl->size())
			{
				throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError);
			}

			const std::string& command = (*impl)[0].asString();
			auto itr = self->handlers.find(command);
			if(itr != self->handlers.end())
			{
				itr->second(*impl);
			}
			self->streambuf.consume(size);
			self->AsyncRead();
		})); 
	}

	bool Subscriber::Subscribe(const std::string& channel, const std::function<void(const std::string& message)>& callback)
	{
		auto itr = callback_functions.find(channel);
		if(callback_functions.end() == itr)
		{
			AsyncSend("SUBSCRIBE " + channel);
		}
		callback_functions[channel] += callback;
		return true;
	}

	void Subscriber::Unsubscribe(const std::string& channel)
	{
		AsyncSend("UNSUBSCRIBE " + channel);
		callback_functions.erase(channel);
	}


	void Subscriber::OnRecv_SubscribeAns(const Json::Value& ans)
	{
		if(3 != ans.size())
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError);
		}
	}

	void Subscriber::OnRecv_PublishReq(const Json::Value& req)
	{
		if(3 != req.size())
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError);
		}

		const std::string& channel = req[1].asString();

		auto itr = callback_functions.find(channel);
		if(callback_functions.end() != itr)
		{
			const std::string& message = req[2].asString();
			itr->second(message);
		}
	}
}}}
