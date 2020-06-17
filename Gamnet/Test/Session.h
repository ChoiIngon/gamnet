#ifndef GAMNET_TEST_SESSION_H_
#define GAMNET_TEST_SESSION_H_

#include "../Library/Debugs.h"
#include "../Library/Time/Time.h"
#include "../Network/Tcp/Session.h"
#include "../Network/Tcp/Connector.h"

namespace Gamnet { namespace Test {

class Session : public Network::Tcp::Session 
{
	Network::Tcp::Connector reconnector;
public:
	Session();
	virtual ~Session();

	std::string	host;
	int	port;
	int test_seq;
	Time::ElapseTimer elapse_timer;

	virtual void OnCreate() override {}
	virtual void OnAccept() override {}
	virtual void OnConnect() {}
	virtual void OnClose(int reason) override {}

	virtual bool Init() override;
	virtual void Clear() override;

	void AsyncSend(const std::shared_ptr<Network::Tcp::Packet>& packet);
	void Next();

	void Send_Connect_Req();
	void Recv_Connect_Ans(const std::shared_ptr<Network::Tcp::Packet>& packet);
	void Send_Reconnect_Req();
	void Recv_Reconnect_Ans(const std::shared_ptr<Network::Tcp::Packet>& packet);
	void Send_ReliableAck_Ntf();
	void Send_Close_Req();
	void Recv_Close_Ans(const std::shared_ptr<Network::Tcp::Packet>& packet);
private:
	void OnReconnect(const std::shared_ptr<boost::asio::ip::tcp::socket>& socket);

	
};

}} /* namespace Gamnet */

#endif /* TESTSESSION_H_ */
