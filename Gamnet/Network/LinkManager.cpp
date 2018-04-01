#include "LinkManager.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Network {

std::atomic_ulong LinkManager::link_key;

LinkManager::LinkManager() :
	_name("Gamnet::Network::LinkManager"),
	_is_acceptable(true),
	_acceptor(Singleton<boost::asio::io_service>::GetInstance())
{
}

LinkManager::~LinkManager()
{
}

void LinkManager::Listen(int port)
{
	_endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
	_acceptor.open(_endpoint.protocol());
	_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	_acceptor.bind(_endpoint);
	_acceptor.listen();
	Accept();
}

void LinkManager::Accept()
{
	std::shared_ptr<Link> link = Create();
	if(nullptr == link)
	{
		LOG(GAMNET_ERR, "[link_manager:", _name, "] can not create link(pool_size:", Available(), "). deny addtional connection");
		std::lock_guard<std::mutex> lo(_lock);
		_is_acceptable = false;
		return;
	}

	_acceptor.async_accept(link->socket, link->strand.wrap(
		boost::bind(&LinkManager::Callback_Accept, this, link, boost::asio::placeholders::error)
	));
}

void LinkManager::Callback_Accept(const std::shared_ptr<Link>& link, const boost::system::error_code& error)
{
	LOG(DEV, "[link_key:", link->link_key, "] error_code:", error.value());
	Accept();

	if(0 == error)
	{
		try {
			boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
			link->socket.set_option(option);
			link->remote_address = link->socket.remote_endpoint().address();
			link->AsyncRead();

			OnAccept(link);

			std::lock_guard<std::mutex> lo(_lock);
			_links.insert(std::make_pair(link->link_key, link));
		}
		catch(const boost::system::system_error& e)
		{
			LOG(GAMNET_ERR, "[link_key:", link->link_key, "] accept fail(errno:", e.code().value(), ", errstr:", e.what(), ")");
			link->Close(ErrorCode::AcceptFailError);
		}
	}
	else
	{
		LOG(GAMNET_ERR, "[link_key:", link->link_key, "] accept fail(errno:", error, ")");
		link->Close(ErrorCode::AcceptFailError);
	}
}

std::shared_ptr<Link> LinkManager::Connect(const char* host, int port, int timeout)
{
	std::shared_ptr<Link> link = Create();
	if(nullptr == link)
	{
		LOG(GAMNET_ERR, "[link_manager:", _name, "] can not create link. connect fail(pool_size:", Available(), ", host:", host, ", port:", port, ", timeout:", timeout);
		return nullptr;
	}

	{
		std::lock_guard<std::mutex> lo(_lock);
		_links.insert(std::make_pair(link->link_key, link));
	}
	link->Connect(host, port, timeout);
	
	return link;
}


void LinkManager::Remove(uint32_t linkKey)
{
	std::lock_guard<std::mutex> lo(_lock);
	_links.erase(linkKey);
	if(false == _is_acceptable)
	{
		if(0 < Available())
		{
			_is_acceptable = true;
			Accept();
		}
	}
}

size_t LinkManager::Size()
{
	return 0; 
}

size_t LinkManager::Available()
{
	return 0; 
}

size_t LinkManager::Capacity() 
{
	return 0; 
}

Json::Value LinkManager::State()
{
	Json::Value root;

	time_t logtime_;
	struct tm when;
	time(&logtime_);

	char date_time[22] = { 0 };
#ifdef _WIN32
	localtime_s(&when, &logtime_);
	_snprintf_s(date_time, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#else
	localtime_r(&logtime_, &when);
	snprintf(date_time, 20, "%04d-%02d-%02d %02d:%02d:%02d", when.tm_year + 1900, when.tm_mon + 1, when.tm_mday, when.tm_hour, when.tm_min, when.tm_sec);
#endif
	root["date_time"] = date_time;
	
	Json::Value link;
	link["capacity"] = (unsigned int)Capacity();
	link["available"] = (unsigned int)Available();
	link["running_count"] = (unsigned int)Size();
	root["link"] = link;
	return root;
}

}} 
