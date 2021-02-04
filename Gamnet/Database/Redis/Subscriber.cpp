#include "Subscriber.h"
#include "ResultSet.h"

namespace Gamnet { namespace Database { namespace Redis {
	Subscriber::Subscriber() 
	{
		connector.connect_handler = std::bind(&Subscriber::OnConnect, this, std::placeholders::_1);
		connector.error_handler = std::bind(&Subscriber::OnError, this, std::placeholders::_1);
		handlers.insert(std::make_pair("subscribe", std::bind(&Subscriber::OnRecv_SubscribeAns, this, std::placeholders::_1)));
		handlers.insert(std::make_pair("message", std::bind(&Subscriber::OnRecv_PublishReq, this, std::placeholders::_1)));
	}

	Subscriber::~Subscriber()
	{
	}

	bool Subscriber::Init()
	{
		if(false == Network::Session::Init())
		{
			return false;
		}

		recv_buffer = Buffer::Create();
		if(nullptr == recv_buffer)
		{
			LOG(ERR, "can not create buffer instance");
			return false;
		}

		return true;
	}

	void Subscriber::Connect(const std::string& host, int port)
	{
		this->host = host;
		this->port = port;
		connector.AsyncConnect(host, port, 5);
	}

	void Subscriber::Reconnect()
	{
		if(nullptr != this->socket)
		{
			return;
		}
		connector.AsyncConnect(host, port, 5);
	}

	void Subscriber::OnConnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket)
	{
		this->socket = socket;
		OnCreate();

		{
			std::lock_guard<std::mutex> lo(lock);
			for(const auto& itr : callback_functions)
			{
				AsyncSend(Format("SUBSCRIBE ", itr.first));
			}
		}

		AsyncRead();
	}

	void Subscriber::OnError(int error)
	{
		reconnect_timer.SetTimer(1000, std::bind(&Subscriber::Reconnect, this));
	}

	void Subscriber::Close(int reason)
	{
		auto self = shared_from_this();
		Dispatch([this, self, reason]() {
			if (nullptr == socket)
			{
				return;
			}
			OnClose(reason);
			OnDestroy();
			socket = nullptr;
			
			reconnect_timer.SetTimer(1000, std::bind(&Subscriber::Reconnect, this));
		});
	}

	void Subscriber::AsyncSend(const std::string& query)
	{
		std::shared_ptr<Buffer> buffer = Buffer::Create();
		if (nullptr == buffer)
		{
			LOG(GAMNET_ERR, "can not create buffer(session_key:", session_key, ")");
			return;
		}
		buffer->Append(query.c_str(), query.length());
		buffer->Append("\r\n", 2);
		Network::Session::AsyncSend(buffer);
	}

	void Subscriber::OnRead(const std::shared_ptr<Buffer>& buffer)
	{
		recv_buffer->Append(buffer->ReadPtr(), buffer->Size());
		
		std::shared_ptr<ResultSetImpl> impl = std::make_shared<ResultSetImpl>();
		if(false == impl->Parse(std::string(recv_buffer->ReadPtr(), recv_buffer->Size())))
		{
			return;
		}

		recv_buffer->Remove(impl->read_index);

		if (1 > impl->size())
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError, impl->error);
		}

		const std::string& command = (*impl)[0].asString();

		std::lock_guard<std::mutex> lo(lock);
		auto itr = handlers.find(command);
		if (itr != handlers.end())
		{
			itr->second(*impl);
		}
	}

	void Subscriber::Subscribe(const std::string& channel, const std::function<void(const std::string& message)>& callback)
	{
		{
			std::lock_guard<std::mutex> lo(lock);
			auto itr = callback_functions.find(channel);
			if(callback_functions.end() != itr)
			{
				throw GAMNET_EXCEPTION(ErrorCode::DuplicateMessageIDError, "duplicated subscribe hander(channel:", channel, ")");
			}
			callback_functions[channel] = callback;
		}

		AsyncSend(Format("SUBSCRIBE ", channel));
	}

	void Subscriber::Unsubscribe(const std::string& channel)
	{
		{
			std::lock_guard<std::mutex> lo(lock);
			callback_functions.erase(channel);
		}
		AsyncSend(Format("UNSUBSCRIBE ", channel));
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
