#ifndef GAMNET_NETWORK_ACCEPTOR_H_
#define GAMNET_NETWORK_ACCEPTOR_H_

#include "../Library/Pool.h"
#include "../Library/Debugs.h"
#include "Link.h"

namespace Gamnet { namespace Network {

template <class LINK_T>
class Acceptor
{
	//GAMNET_WHERE(LINK_T, Link);
private :
	struct InitFunctor
	{
		template<class T>
		T* operator() (T* link)
		{
			if (false == link->Init())
			{
				return nullptr;
			}
			return link;
		}
	};
	struct ReleaseFunctor
	{
		template<class T>
		T* operator() (T* link)
		{
			if (nullptr != link)
			{
				link->Clear();
			}
			//Singleton<Acceptor<LINK_T>>::GetInstance().Release();
			return link;
		}
	};
	
	Pool<LINK_T, std::mutex, InitFunctor, ReleaseFunctor> _link_pool;
	typedef typename Pool<LINK_T, std::mutex, InitFunctor, ReleaseFunctor>::object_factory object_factory;

	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::endpoint _endpoint;

	int _max_accept_size;
	std::atomic<int> _cur_accept_size;
public :
	Acceptor(int nSize, object_factory factory) :
		_link_pool(nSize, factory),
		_acceptor(Singleton<boost::asio::io_service>::GetInstance()),
		_max_accept_size(0),
		_cur_accept_size(0)
	{
	}

	bool Listen(int port, int max_count, int queue_size)
	{
		_link_pool.Capacity(max_count);
		_endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
		_acceptor.open(_endpoint.protocol());
		_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		_acceptor.bind(_endpoint);
		_acceptor.listen();

		_max_accept_size = queue_size;
		for (int i = 0; i<_max_accept_size; i++)
		{
			_cur_accept_size++;
			Accept();
		}
		
		return true;
	}

	void Accept()
	{
		std::shared_ptr<Network::Link> link = _link_pool.Create();
		if (nullptr == link)
		{
			LOG(GAMNET_ERR, "can not create link. deny addtional connection");
			_cur_accept_size = std::max(_cur_accept_size - 1, 0);
			return;
		}

		_acceptor.async_accept(link->socket, boost::bind(&Acceptor::Callback_Accept, this, link, boost::asio::placeholders::error));
	}

	void Callback_Accept(const std::shared_ptr<Link> link, const boost::system::error_code& ec)
	{
		Accept();

		try {
			if (0 != ec)
			{
				throw GAMNET_EXCEPTION(ErrorCode::AcceptFailError, "link_key:", link->link_key, ", error_code:", ec.value());
			}

			boost::asio::socket_base::send_buffer_size option(Buffer::MAX_SIZE);
			link->socket.set_option(option);
			link->remote_address = link->socket.remote_endpoint().address();
			link->OnAccept();
			link->AsyncRead();
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

	void Release()
	{
		if(_cur_accept_size < _max_accept_size)
		{
			_cur_accept_size++;
			Accept();
		}
	}
};

}}
#endif
