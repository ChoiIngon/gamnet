#include "Connection.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database {	namespace Redis {

	static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

	Connection::Connection() 
	{
	}

	Connection::~Connection() 
	{
	}

	bool Connection::Connect(const ConnectionInfo& connInfo)
	{
		connection_info = connInfo;

		timer.Cancel();
		timer.AutoReset(false);
		timer.SetTimer(5000, strand.wrap(std::bind(&Link::Close, this, ErrorCode::ConnectTimeoutError)));

		boost::asio::ip::address remote_address;
		boost::asio::ip::tcp::resolver resolver_(io_service_);
		boost::asio::ip::tcp::resolver::query query_(connection_info.host, "");
		for (auto itr = resolver_.resolve(query_); itr != boost::asio::ip::tcp::resolver::iterator(); ++itr)
		{
			boost::asio::ip::tcp::endpoint end = *itr;
			remote_address = end.address();
			break;
		}
		LOG(INF, "[Redis] connect...(host:", remote_address.to_v4().to_string(), ", port:", connection_info.port, ")");
		boost::asio::ip::tcp::endpoint endpoint_(*resolver_.resolve({ remote_address.to_v4().to_string(), Format(connection_info.port).c_str() }));
		boost::system::error_code ec;
		socket.connect(endpoint_, ec);
		if (0 != ec)
		{
			LOG(GAMNET_ERR, "[Redis] connect fail(host:", connection_info.host, ", port:", connection_info.port, ")");
			return false;
		}

		timer.Cancel();
		return true;
	}

	bool Connection::Reconnect()
	{
		return Connect(connection_info);
	}

	std::shared_ptr<ResultSetImpl> Connection::Execute(const std::string& query)
	{	
		if (false == socket.is_open())
		{
			Reconnect();
		}
	
		try 
		{
			const std::string sendBytes = query + "\r\n";
			int sentBytes = SyncSend(sendBytes.c_str(), sendBytes.length());
			if(0 > sentBytes)
			{
				throw GAMNET_EXCEPTION(ErrorCode::SendMsgFailError);
			}

			std::vector<char> buffer;
			std::shared_ptr<ResultSetImpl> impl(new ResultSetImpl(std::static_pointer_cast<Connection>(shared_from_this())));
			do {
				char data[1024 * 6] = { 0 };
				size_t readBytes = socket.read_some(boost::asio::buffer(data, 1024 * 6));
				buffer.insert(buffer.end(), &data[0], &data[readBytes]);
			} while (false == impl->Parse(std::string(&buffer[0], buffer.size())));

			if ("" != impl->error)
			{
				throw GAMNET_EXCEPTION(ErrorCode::RedisResultError, impl->error);
			}

			return impl;
		}
		catch(const boost::system::system_error& e)
		{
			LOG(ERR, "error_code:", e.code(), ", reason:", e.what());
		}
		
		socket.close();
		return nullptr;
	}

	
}}}
