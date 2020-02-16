#include "Subscriber.h"

namespace Gamnet { namespace Database { namespace Redis {

	Subscriber::Subscriber(Network::LinkManager* linkManager) : Network::Link(linkManager)
	{
		handlers.insert(std::make_pair("subscribe", std::bind(&Subscriber::OnRecv_SubscribeAns, this, std::placeholders::_1)));
		handlers.insert(std::make_pair("message", std::bind(&Subscriber::OnRecv_PublishReq, this, std::placeholders::_1)));
	}

	Subscriber::~Subscriber()
	{
	}

	bool Subscriber::Init()
	{
		if(false == Network::Link::Init())
		{
			return false;
		}

		recv_buffer = Buffer::Create();
		if(nullptr == recv_buffer)
		{
			LOG(ERR, "[", link_manager->name, "/", link_key, "/0] can not create buffer instance");
			return false;
		}

		return true;
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

	void Subscriber::OnRead(const std::shared_ptr<Buffer>& buffer)
	{
		int start = std::max(0, (int)recv_buffer->Size() - 1);
		recv_buffer->Append(buffer->ReadPtr(), buffer->Size());
		
		std::shared_ptr<ResultSetImpl> impl = std::make_shared<ResultSetImpl>();
		if(false == impl->Parse(std::string(recv_buffer->ReadPtr(), recv_buffer->Size())))
		{
			return;
		}

		recv_buffer->Remove(impl->read_index);

		if (1 > impl->size())
		{
			throw GAMNET_EXCEPTION(ErrorCode::MessageFormatError);
		}

		const std::string& command = (*impl)[0].asString();
		auto itr = handlers.find(command);
		if (itr != handlers.end())
		{
			itr->second(*impl);
		}
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

	SubscriberManager::SubscriberManager() 
	{
		name = "Gamnet::Database::Redis::SubscriberManager";
	}

	std::shared_ptr<Network::Link> SubscriberManager::Create()
	{
		std::shared_ptr<Subscriber> subscriber = std::make_shared<Subscriber>(this);
		if (nullptr == subscriber)
		{
			LOG(GAMNET_ERR, "[link_manager:", name, "] can not create 'Tcp::Link' instance");
			return nullptr;
		}
		if(false == subscriber->Init())
		{
			return nullptr;
		}
		return subscriber;
	}
}}}
