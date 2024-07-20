#include "Connection.h"
#include "../../Library/Exception.h"
#include "../../Log/Log.h"
import Gamnet.Singleton;

namespace Gamnet::Database {
namespace Redis {

	Connection::Connection() 
	{
#ifdef _DEBUG
		connector.name = "Redis::Connection";
#endif
		connector.connect_handler = std::bind(&Connection::OnConnect, this, std::placeholders::_1);
	}

	Connection::~Connection() 
	{
	}

	bool Connection::Connect(const ConnectionInfo& connInfo)
	{
		connection_info = connInfo;
		return connector.SyncConnect(connection_info.host, connection_info.port, 5);
	}

	bool Connection::Reconnect()
	{
		return Connect(connection_info);
	}

	std::shared_ptr<ResultSetImpl> Connection::Execute(const std::string& query)
	{	
		if (nullptr == socket)
		{
			if (false == Reconnect())
			{
				return nullptr;
			}
		}
	
		try 
		{
			int totalSentBytes = 0;
			const std::string send_buffer = query + "\r\n";
			while ((int)send_buffer.length() > totalSentBytes)
			{
				boost::system::error_code ec;
				int sentBytes = boost::asio::write(*(socket), boost::asio::buffer(&send_buffer[0] + totalSentBytes, send_buffer.length() - totalSentBytes), ec);
				if (0 > sentBytes || 0 != ec.value())
				{
					throw GAMNET_EXCEPTION(ErrorCode::SendMsgFailError, "errno:", errno, ", ec:", ec);
				}
				if (0 == sentBytes)
				{
					throw GAMNET_EXCEPTION(ErrorCode::SendMsgFailError, "errno:", errno, ", ec:", ec);
				}
				totalSentBytes += sentBytes;
			}
			
			std::vector<char> recv_buffer;
			std::shared_ptr<ResultSetImpl> impl(new ResultSetImpl(std::static_pointer_cast<Connection>(shared_from_this())));
			do {
				char data[1024 * 6] = { 0 };
				size_t readBytes = socket->read_some(boost::asio::buffer(data, 1024 * 6));
				recv_buffer.insert(recv_buffer.end(), &data[0], &data[readBytes]);
			} while (false == impl->Parse(std::string(&recv_buffer[0], recv_buffer.size())));

			if ("" != impl->error)
			{
				throw GAMNET_EXCEPTION(ErrorCode::RedisResultError, impl->error);
			}

			return impl;
		}
		catch (const Gamnet::Exception& e)
		{
			LOG(ERR, "error_code:", e.error_code(), ", reason:", e.what());
		}
		catch(const boost::system::system_error& e)
		{
			LOG(ERR, "error_code:", e.code(), ", reason:", e.what());
		}
		
		socket = nullptr;
		return nullptr;
	}

	void Connection::OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
	{
		this->socket = socket;
		LOG(INF, "[Gamnet::Database::Redis] connect...(db_num:", connection_info.db_num, ", host:", connection_info.host, ", port:", connection_info.port, ")");
	}
}}
