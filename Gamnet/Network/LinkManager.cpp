#include "LinkManager.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Network {

std::atomic_ullong LinkManager::link_key;

LinkManager::LinkManager() :
		_keepalive_time(0),
		_is_acceptable(true),
		_acceptor(Singleton<boost::asio::io_service>::GetInstance())
{
}

LinkManager::~LinkManager()
{
}

void LinkManager::Listen(int port, int max_session, int keep_alive_sec)
{
	if(0 < keep_alive_sec)
	{
		_keepalive_time = keep_alive_sec;
		if (false == _timer.SetTimer(5000, [this](){
			std::lock_guard<std::recursive_mutex> lo(_lock);
			time_t now_ = time(NULL);
			for(auto itr = _links.begin(); itr != _links.end();) {
				std::shared_ptr<Link> link = itr->second;
				if(link->expire_time + _keepalive_time < now_)
				{
					LOG(GAMNET_ERR, "[link_key:", link->link_key, "] idle link timeout(ip:", link->remote_address.to_string(), ")");
			        _links.erase(itr++);
			        link->strand.wrap(std::bind(&Link::OnError, link, ETIMEDOUT))();
			    }
			    else {
			        ++itr;
			    }
			}
			_timer.Resume();
		}))
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::UndefinedError), "session time out timer init fail");
		}
	}
	_link_pool.Capacity(max_session);
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
	if(NULL == link)
	{
		LOG(GAMNET_WRN, "can not create any more link(max:", _link_pool.Capacity(), ", current:", Size(), ")");
		std::lock_guard<std::recursive_mutex> lo(_lock);
		_is_acceptable = false;
		return;
	}

	_acceptor.async_accept(link->socket, link->strand.wrap(
		boost::bind(&LinkManager::Callback_Accept, this, link, boost::asio::placeholders::error)
	));
}

void LinkManager::Callback_Accept(const std::shared_ptr<Link>& link, const boost::system::error_code& error)
{
	if(0 == error)
	{
		try {
			boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
			link->socket.set_option(option);
			link->remote_address = link->socket.remote_endpoint().address();
			link->expire_time = ::time(NULL);
			link->AttachManager(this);
			OnAccept(link);
			link->AsyncRead();
		}
		catch(const boost::system::system_error& e)
		{
			LOG(GAMNET_ERR, "[link_key:", link->link_key, "] accept fail(errno:", errno, ", errstr:", e.what(), ")");
		}
	}
	Accept();
}

void LinkManager::OnAccept(const std::shared_ptr<Link>& link)
{
}

void LinkManager::OnConnect(const std::shared_ptr<Link>& link)
{
}

void LinkManager::OnClose(const std::shared_ptr<Link>& link, int reason)
{
	if(false == _is_acceptable)
	{
		std::lock_guard<std::recursive_mutex> lo(_lock);
		if(false == _is_acceptable)
		{
			if(0 < _link_pool.Available())
			{
				_is_acceptable = true;
				Accept();
			}
		}
	}
}

bool LinkManager::Add(uint64_t key, const std::shared_ptr<Link>& link)
{
	std::lock_guard<std::recursive_mutex> lo(_lock);
	if(false == _links.insert(std::make_pair(key, link)).second)
	{
		LOG(ERR, "[link_key:", key, "] duplicated link key");
		return false;
	}
	return true;
}

void LinkManager::Remove(uint64_t key)
{
	if(0 == key)
	{
		return;
	}
	std::lock_guard<std::recursive_mutex> lo(_lock);
	_links.erase(key);
}

std::shared_ptr<Link> LinkManager::Find(uint64_t key)
{
	std::lock_guard<std::recursive_mutex> lo(_lock);
	auto itr = _links.find(key);
	if(_links.end() == itr)
	{
		return NULL;
	}
	return itr->second;
}

size_t LinkManager::Size()
{
	std::lock_guard<std::recursive_mutex> lo(_lock);
	return _links.size();
}

std::shared_ptr<Link> LinkManager::Create()
{
	std::shared_ptr<Link> link = _link_pool.Create();
	if(NULL == link)
	{
		LOG(ERR, "can't create link instance");
		return NULL;
	}
	link->link_key = ++LinkManager::link_key;
	LOG(DEV, "[link_key:", link->link_key,"] create link");
	return link;
}

int LinkManager::Available()
{
	return _link_pool.Available();
}

int LinkManager::Capacity()
{
	return _link_pool.Capacity();
}

void LinkManager::Capacity(int count)
{
	_link_pool.Capacity(count);
}

}} /* namespace Gamnet */
