#include "Connection.h"
#include "../../Library/Singleton.h"
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

namespace Gamnet { namespace Database {	namespace Redis {

	static boost::asio::io_service& io_service_ = Singleton<boost::asio::io_service>::GetInstance();

	class Session : public Network::Session
	{
	public:
		virtual void OnCreate() override {}
		virtual void OnAccept() override {}
		virtual void OnClose(int reason) override {}
		virtual void OnDestroy() override {}
	};

	Connection::Connection() 
	{
		session = std::make_shared<Session>();
	}

	Connection::~Connection() 
	{
	}

	bool Connection::Connect(const ConnectionInfo& connInfo)
	{
		connection_info = connInfo;
		return SyncConnect(connection_info.host.c_str(), connection_info.port, 5);
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
