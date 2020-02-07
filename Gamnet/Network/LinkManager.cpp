#include "LinkManager.h"
#include "../Library/Singleton.h"
#include <list>

namespace Gamnet { namespace Network {

LinkManager::LinkManager() :
	_acceptor(Singleton<boost::asio::io_service>::GetInstance()),
	_max_accept_size(5),
	_keep_alive_time(300),
	_cur_accept_size(5),
	name("Gamnet::Network::LinkManager")
{
}

LinkManager::~LinkManager()
{
}

bool LinkManager::Listen(int port, int accept_queue_size, int keep_alive_time)
{
	_endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
	_acceptor.open(_endpoint.protocol());
	_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	_acceptor.bind(_endpoint);
	_acceptor.listen();

	_max_accept_size = accept_queue_size;
	for(int i=0; i<_max_accept_size; i++)
	{
		if(false == Accept())
		{
			return false;
		}
		std::lock_guard<std::mutex> lo(_lock);
		_cur_accept_size++;
	}
	_keep_alive_time = keep_alive_time;
	expire_timer.Cancel();
	expire_timer.AutoReset(true);
	expire_timer.SetTimer((0 == _keep_alive_time ? 3600 * 1000 : _keep_alive_time * 1000), std::bind(&LinkManager::OnTimerExpire, this));
	return true;
}

bool LinkManager::Accept()
{
	std::shared_ptr<Link> link = Create();
	if(nullptr == link)
	{
		LOG(GAMNET_ERR, "[link_manager:", name, "] can not create link. deny addtional connection");
		std::lock_guard<std::mutex> lo(_lock);
		_cur_accept_size = std::max(_cur_accept_size-1, 0);
		return false;
	}

	_acceptor.async_accept(link->socket, boost::bind(&LinkManager::Callback_Accept, this, link, boost::asio::placeholders::error));

	return true;
}

void LinkManager::Callback_Accept(const std::shared_ptr<Link> link, const boost::system::error_code& ec)
{
	Accept();

	//link->strand.wrap([this, link, ec] () {
		try {
			if(0 != ec)
			{
				throw GAMNET_EXCEPTION(ErrorCode::AcceptFailError, "link_key:", link->link_key, ", error_code:", ec.value());
			}
	
			boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
			link->socket.set_option(option);
	
			link->remote_address = link->socket.remote_endpoint().address();
				
			OnAccept(link);
	
			if(false == Add(link))
			{
				assert(!"duplcated key");
				throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "[link_key:", link->link_key, "] duplicated link");
			}
			link->AsyncRead();
			return;
		}
		catch(const Exception& e)
		{
			LOG(GAMNET_ERR, "[link_key:", link->link_key, "] accept fail(errno:", e.error_code(), ")");
		}
		catch(const boost::system::system_error& e)
		{
			LOG(GAMNET_ERR, "[link_key:", link->link_key, "] accept fail(errno:", e.code().value(), ", errstr:", e.what(), ")");
		}
		link->Close(ErrorCode::AcceptFailError);
	//})();
}

std::shared_ptr<Link> LinkManager::Connect(const char* host, int port, int timeout)
{
	std::shared_ptr<Link> link = Create();
	if(nullptr == link)
	{
		LOG(GAMNET_ERR, "[link_manager:", name, "] can not create link. connect fail(host:", host, ", port:", port, ", timeout:", timeout, ")");
		return nullptr;
	}
	
	link->Connect(host, port, timeout);
	return link;
}

bool LinkManager::Add(const std::shared_ptr<Link>& link)
{
	std::lock_guard<std::mutex> lo(_lock);
	return _links.insert(std::make_pair(link->link_key, link)).second;	
}

void LinkManager::Remove(uint32_t linkKey)
{
	std::lock_guard<std::mutex> lo(_lock);
	_links.erase(linkKey);
	if(_cur_accept_size < _max_accept_size)
	{
		for(int i=_cur_accept_size; i < _max_accept_size; i++)
		{
			if(true == Accept())
			{
				_cur_accept_size++;
			}
			else
			{
				break;
			}
		}
	}
}

size_t LinkManager::Size() 
{
	std::lock_guard<std::mutex> lo(_lock);
	return _links.size();
}

void LinkManager::OnTimerExpire()
{
	std::list<std::shared_ptr<Link>> linksToBeDeleted;
	{
		std::lock_guard<std::mutex> lo(_lock);
		int64_t now_ = time(nullptr);
		if (0 < _keep_alive_time)
		{
			for (auto itr = _links.begin(); itr != _links.end(); itr++) {
				std::shared_ptr<Link> link = itr->second;
				if (link->expire_time + _keep_alive_time < now_)
				{
					linksToBeDeleted.push_back(link);
				}
			}
		}
	}

	for (auto link: linksToBeDeleted)
	{
		LOG(GAMNET_ERR, "[link_key:", link->link_key, "] destroy idle link");
		link->Close(ErrorCode::IdleTimeoutError);
	}
}
}} 
