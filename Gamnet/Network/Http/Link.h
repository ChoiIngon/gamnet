#ifndef GAMNET_NETWORK_HTTP_LINK_H_
#define GAMNET_NETWORK_HTTP_LINK_H_

#include "../Link.h"

namespace Gamnet { namespace Network { namespace Http {

	class Link : public Network::Link 
	{
		std::shared_ptr<Buffer> recv_buffer;
	public :
		Link();
		virtual ~Link();

		std::shared_ptr<Session> session;

		virtual bool Init() override;
		virtual void OnAccept() override;
		virtual void OnRead(const std::shared_ptr<Buffer>& buffer) override;
	};

}}}
#endif
