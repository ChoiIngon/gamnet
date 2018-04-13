#include "LinkManager.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Network {

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
	for(int i=0; i<5; i++)
	{
		Accept();
	}
}

bool LinkManager::Accept()
{
	std::shared_ptr<Link> link = Create();
	if(nullptr == link)
	{
		LOG(GAMNET_ERR, "[link_manager:", _name, "] can not create link. deny addtional connection");
		std::lock_guard<std::mutex> lo(_lock);
		_is_acceptable = false;
		return false;
	}

	_acceptor.async_accept(link->socket, link->strand.wrap(
		boost::bind(&LinkManager::Callback_Accept, this, link, boost::asio::placeholders::error)
	));

	return true;
}

void LinkManager::Callback_Accept(const std::shared_ptr<Link>& link, const boost::system::error_code& error)
{
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
		LOG(GAMNET_ERR, "[link_manager:", _name, "] can not create link. connect fail(host:", host, ", port:", port, ", timeout:", timeout, ")");
		return nullptr;
	}

	link->Connect(host, port, timeout);

	std::lock_guard<std::mutex> lo(_lock);
	_links.insert(std::make_pair(link->link_key, link));
	return link;
}

void LinkManager::Remove(uint32_t linkKey)
{
	std::lock_guard<std::mutex> lo(_lock);
	_links.erase(linkKey);
	if(false == _is_acceptable)
	{
		if(true == Accept())
		{
			_is_acceptable = true;
		}
	}
}

}} 
