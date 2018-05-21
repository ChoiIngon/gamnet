#ifndef _GAMNET_TEST_LINK_H_
#define _GAMNET_TEST_LINK_H_

#include "../Network/Network.h"

namespace Gamnet { namespace Test {
	class Link : public Network::Tcp::Link
	{
		bool is_open;
	public :
		Link(Network::LinkManager* linkManager);
		virtual ~Link();

		virtual bool Init() override;

		virtual void Close(int reason) override;
	protected :
		virtual void OnConnect(const boost::system::error_code& ec) override;
		virtual void OnSend(const boost::system::error_code& ec, std::size_t transferredBytes) override;
	};
}}
#endif
