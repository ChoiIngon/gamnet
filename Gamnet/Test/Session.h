#ifndef GAMNET_TEST_SESSION_H_
#define GAMNET_TEST_SESSION_H_

#include "../Library/Debugs.h"
#include "../Library/Time/Time.h"
#include "../Network/Tcp/Session.h"
#include "../Network/Tcp/Connector.h"

namespace Gamnet { namespace Test {

class Session : public Network::Tcp::Session
{
public :
	Session();
	virtual ~Session();

	virtual bool Init() override;
	virtual void Close(int reason) override;

	virtual void OnConnect() = 0;

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
	virtual void OnAccept() override {}

private :
	Network::Tcp::Connector reconnector;

public:
	std::string	host;
	int	port;
	int test_seq;
	Time::ElapseTimeCounter elapse_time;
};

}} /* namespace Gamnet */

#endif /* TESTSESSION_H_ */
