#ifndef GAMNET_TEST_SESSION_H_
#define GAMNET_TEST_SESSION_H_

#include "../Library/Debugs.h"
#include "../Library/Timer.h"
#include "../Network/Tcp/Session.h"

namespace Gamnet { namespace Test {

class Session : public Network::Tcp::Session 
{
public:
	Session();
	virtual ~Session();

	std::string	host;
	int	port;
	int test_seq;
	uint64_t send_time;

	virtual void OnCreate() override {}
	virtual void OnAccept() override {}
	virtual void OnConnect() {}
	virtual void OnClose(int reason) override {}

	virtual bool Init() override;
	virtual void Clear() override;

	void AsyncSend(const std::shared_ptr<Network::Tcp::Packet>& packet);
	void Next();

private:
	void AsyncConnect();
	void Callback_Connect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::shared_ptr<Time::Timer>& timer, const boost::asio::ip::tcp::endpoint& endpoint, const boost::system::error_code& ec);
	void Callback_ConnectTimeout(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket, const std::shared_ptr<Time::Timer>& timer);
};

}} /* namespace Gamnet */

#endif /* TESTSESSION_H_ */
