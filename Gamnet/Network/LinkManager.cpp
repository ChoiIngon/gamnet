#include "LinkManager.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Network {

std::atomic_ullong LinkManager::link_key;

LinkManager::LinkManager() :
		name("Gamnet::Network::LinkManager"),
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
			        link->strand.wrap(std::bind(&Link::OnError, link, ErrorCode::IdleTimeoutError))();
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
		LOG(GAMNET_INF, "can not create link(pool_size:", Available(), "), link manager will deny addtional conntion");
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
	Accept();

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
			link->OnError(ErrorCode::AcceptFailError);
			LOG(GAMNET_ERR, "[link_key:", link->link_key, "] accept fail(errno:", e.code().value(), ", errstr:", e.what(), ")");
		}
	}
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
			LOG(GAMNET_INF, "new connection is available");
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
	if(nullptr == link)
	{
		return nullptr;
	}

	if (nullptr == link->read_buffer)
	{
		LOG(GAMNET_ERR, "can not create packet");
		return nullptr;
	}

	link->link_key = ++LinkManager::link_key;
	//LOG(DEV, "[link_key:", link->link_key,"] create link");
	return link;
}

size_t LinkManager::Available()
{
	return _link_pool.Available();
}

size_t LinkManager::Capacity() const
{
	return _link_pool.Capacity();
}

void LinkManager::Capacity(size_t count)
{
	_link_pool.Capacity(count);
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
	root["keepalive_time"] = _keepalive_time;
	
	Json::Value link;
	link["capacity"] = (unsigned int)Capacity();
	link["available"] = (unsigned int)Available();
	link["running_count"] = (unsigned int)Size();
	root["link"] = link;
	return root;
}
}} /* namespace Gamnet */
