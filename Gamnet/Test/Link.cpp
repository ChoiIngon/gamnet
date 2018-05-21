#include "Link.h"

namespace Gamnet { namespace Test {
	Link::Link(Network::LinkManager* linkManager) : Network::Tcp::Link(linkManager), is_open(false)
	{
	}

	Link::~Link()
	{
	}

	bool Link::Init()
	{
		if(false == Network::Tcp::Link::Init())
		{
			return false;
		}
		is_open = false;
		return true;
	}

	void Link::OnConnect(const boost::system::error_code& ec)
	{
		Network::Tcp::Link::OnConnect(ec);
		if(0 == ec)
		{
			is_open = true;
		}
	}

	void Link::OnSend(const boost::system::error_code& ec, std::size_t transferredBytes)
	{
		if(true == send_buffers.empty())
		{
			return;
		}

		send_buffers.pop_front();

		if (false == is_open && true == send_buffers.empty())
		{
			Network::Tcp::Link::Close(ErrorCode::Success);
			return;
		}

		FlushSend();
	}

	void Link::Close(int reason)
	{
		is_open = false;
		if(true == send_buffers.empty())
		{
			Network::Tcp::Link::Close(reason);
			return;
		}
	}
}}