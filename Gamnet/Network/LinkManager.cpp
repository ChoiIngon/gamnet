#include "LinkManager.h"
#include "../Library/Singleton.h"

namespace Gamnet { namespace Network {

std::atomic_ulong LinkManager::link_key;

LinkManager::LinkManager() :
		_name("Gamnet::Network::LinkManager"),
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
	/*
	if(0 < keep_alive_sec)
	{
		_keepalive_time = keep_alive_sec;
		_timer.AutoReset(true);
		if (false == _timer.SetTimer(5000, [this](){
			std::lock_guard<std::recursive_mutex> lo(_lock);
			time_t now_ = time(nullptr);
			for(auto itr = _links.begin(); itr != _links.end();) {
				std::shared_ptr<Link> link = itr->second;
				if(link->expire_time + _keepalive_time < now_)
				{
					LOG(GAMNET_ERR, "[link_key:", link->link_key, "] idle link timeout(ip:", link->remote_address.to_string(), ")");
			        _links.erase(itr++);
			        link->strand.wrap(std::bind(&Link::Close, link, ErrorCode::IdleTimeoutError))();
			    }
			    else {
			        ++itr;
			    }
			}
		}))
		{
			throw Exception(GAMNET_ERRNO(ErrorCode::UndefinedError), "session time out timer init fail");
		}
	}
	*/
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
		LOG(GAMNET_INF, "[link_manager:", _name, ", link_key:", link->link_key, "] can not create link(pool_size:", Available(), "), link manager(name:", _name, ") will deny addtional conntion");
		std::lock_guard<std::recursive_mutex> lo(_lock);
		_is_acceptable = false;
		return;
	}

	if(nullptr == link->session)
	{
		LOG(GAMNET_ERR, "[link_manager:", _name, ", link_key:", link->link_key, "] link refers null session pointer");
		assert(link->session);
	}

	_acceptor.async_accept(link->socket, link->session->strand.wrap(
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
			link->expire_time = ::time(nullptr);
			link->AsyncRead();

			//Add(link->link_key, link);
			OnAccept(link);
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
			if(0 < Available())
			{
				_is_acceptable = true;
				Accept();
			}
		}
	}
}

/*
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
*/
size_t LinkManager::Size()
{
	//std::lock_guard<std::recursive_mutex> lo(_lock);
	return 0; // _links.size();
}

size_t LinkManager::Available()
{
	return 0; // _link_pool.Available();
}

size_t LinkManager::Capacity() const
{
	return 0; // _link_pool.Capacity();
}
/*
void LinkManager::Capacity(size_t count)
{
	// _link_pool.Capacity(count);
}
*/
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

}} 
