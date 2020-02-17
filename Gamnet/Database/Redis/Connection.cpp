#include "Connection.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database {	namespace Redis {

	static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

	Connection::Connection() : socket_(io_service_), deadline_(io_service_)
	{
	}

	Connection::~Connection() 
	{
	}

	bool Connection::Connect(const ConnectionInfo& connInfo)
	{
		deadline_.expires_from_now(boost::posix_time::seconds(5));
		deadline_.async_wait([this](boost::system::error_code e) {
			//if (this->deadline_.expires_at() <= boost::asio::deadline_timer::traits_type::now())
			//{
				// The deadline has passed. The socket is closed so that any outstanding
				// asynchronous operations are cancelled. This allows the blocked
				// connect(), read_line() or write_line() functions to return.
			if (boost::asio::error::operation_aborted == e)
			{
				return;
			}
			boost::system::error_code ignored_ec;
			this->socket_.close(ignored_ec);

				// There is no longer an active deadline. The expiry is set to positive
				// infinity so that the actor takes no action until a new deadline is set.
			this->deadline_.expires_at(boost::posix_time::pos_infin);
			//}
		}); 

		boost::asio::ip::address remote_address;
		boost::asio::ip::tcp::resolver resolver_(io_service_);
		boost::asio::ip::tcp::resolver::query query_(connInfo.host, "");
		for (auto itr = resolver_.resolve(query_); itr != boost::asio::ip::tcp::resolver::iterator(); ++itr)
		{
			boost::asio::ip::tcp::endpoint end = *itr;
			remote_address = end.address();
			break;
		}
		LOG(INF, "[Redis] connect...(host:", remote_address.to_v4().to_string(), ", port:", connInfo.port, ")");
		boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({ remote_address.to_v4().to_string(), Format(connInfo.port).c_str() }));

		boost::system::error_code ec;
		socket_.connect(endpoint_, ec);
		if (0 != ec)
		{
			LOG(GAMNET_ERR, "[Redis] connect fail(host:", connInfo.host, ", port:", connInfo.port, ")");
			return false;
		}

		connection_info = connInfo;
		deadline_.cancel();
		return true;
	}

	bool Connection::Reconnect()
	{
		return Connect(connection_info);
	}

	std::shared_ptr<ResultSetImpl> Connection::Execute(const std::string& query)
	{	
		if (false == socket_.is_open())
		{
			//throw GAMNET_EXCEPTION(ErrorCode::ConnectFailError);
			Reconnect();
		}
	
		try 
		{
			int sentBytes = Send(query + "\r\n");
			if(0 > sentBytes)
			{
				throw GAMNET_EXCEPTION(ErrorCode::SendMsgFailError);
			}

			std::vector<char> buffer;
			std::shared_ptr<ResultSetImpl> impl(new ResultSetImpl(shared_from_this()));
			do {
				char data[1024 * 6] = { 0 };
				size_t readBytes = socket_.read_some(boost::asio::buffer(data, 1024 * 6));
				buffer.insert(buffer.end(), &data[0], &data[readBytes]);
			} while (false == impl->Parse(std::string(&buffer[0], buffer.size())));
			return impl;
		}
		catch (const std::exception& e)
		{
			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
			socket_.close();
			throw e;
		}
	}

	int Connection::Send(const std::string& query)
	{
		int totalSentBytes = 0;
		while (query.length() > totalSentBytes)
		{
			try {
				boost::system::error_code ec;
				int sentBytes = boost::asio::write(socket_, boost::asio::buffer(query.c_str() + totalSentBytes, query.length() - totalSentBytes), ec);
				if (0 > sentBytes || 0 != ec)
				{
					LOG(ERR, "[REDIS/host:", connection_info.host, "/port:", connection_info.port, "] send fail(query:", query, ", ec:", ec, ")");
					return -1;
				}
				if (0 == sentBytes)
				{
					LOG(ERR, "[REDIS/host:", connection_info.host, "/port:", connection_info.port, "] send fail(query:", query, ", ec:", ec, ")");
					return -1;
				}
				totalSentBytes += sentBytes;
			}
			catch (const boost::system::system_error& e)
			{
				LOG(ERR, "[REDIS/host:", connection_info.host, "/port:", connection_info.port, "] send fail(query:", query, ", exception:", e.what(), ")");
				return -1;
			}
		}
		return totalSentBytes;
	}
}}}
