#ifndef GAMNET_NETWORK_ACCEPTOR_H_
#define GAMNET_NETWORK_ACCEPTOR_H_

#include "../Library/Pool.h"
#include "../Library/Debugs.h"
#include "Link.h"

namespace Gamnet { namespace Network {

template <class LINK_T>
class Acceptor
{
private :
	GAMNET_WHERE(LINK_T, Link);
	typedef Pool<LINK_T, std::mutex, Link::InitFunctor, Link::ReleaseFunctor> LINK_POOL_T;
	typedef typename LINK_POOL_T::object_factory object_factory;

	LINK_POOL_T						link_pool;
	boost::asio::ip::tcp::acceptor	acceptor;
	boost::asio::ip::tcp::endpoint	endpoint;

	int								max_accept_size;
	std::atomic<int>				cur_accept_count;
public :
	Acceptor(int nSize = 65535, object_factory factory = Policy::Factory::create<LINK_T>()) :
		link_pool(nSize, factory),
		acceptor(Singleton<boost::asio::io_service>::GetInstance()),
		max_accept_size(0),
		cur_accept_count(0)
	{
	}

	bool Listen(int port, int max_count, int queue_size)
	{
		link_pool.Capacity(max_count);
		endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor.open(endpoint.protocol());
		acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor.bind(endpoint);
		acceptor.listen();

		max_accept_size = queue_size;
		for (int i = 0; i<max_accept_size; i++)
		{
			cur_accept_count++;
			Accept();
		}
		
		return true;
	}

	void Release()
	{
		if (cur_accept_count < max_accept_size)
		{
			cur_accept_count++;
			Accept();
		}
	}

	size_t Capacity() const
	{
		return link_pool.Capacity();
	}

	size_t Available()
	{
		return link_pool.Available();
	}
private :
	void Accept()
	{
		std::shared_ptr<Network::Link> link = link_pool.Create();
		if (nullptr == link)
		{
			LOG(GAMNET_ERR, "can not create link. deny addtional connection");
			cur_accept_count = std::max(cur_accept_count - 1, 0);
			return;
		}

		acceptor.async_accept(link->socket, boost::bind(&Acceptor::Callback_Accept, this, link, boost::asio::placeholders::error));
	}

	void Callback_Accept(const std::shared_ptr<Link> link, const boost::system::error_code& ec)
	{
		Accept();

		try {
			if (0 != ec)
			{
				throw GAMNET_EXCEPTION(ErrorCode::AcceptFailError, "link_key:", link->link_key, ", error_code:", ec.value());
			}

			link->OnAcceptHandler();
			return;
		}
		catch (const Exception& e)
		{
			LOG(GAMNET_ERR, "accept fail(errno:", e.error_code(), ")");
		}
		catch (const boost::system::system_error& e)
		{
			LOG(GAMNET_ERR, "accept fail(errno:", e.code().value(), ", errstr:", e.what(), ")");
		}
		link->Close(ErrorCode::AcceptFailError);
	}
};

}}
#endif
