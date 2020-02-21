#ifndef GAMNET_TEST_SESSION_H_
#define GAMNET_TEST_SESSION_H_
#include "../Network/Network.h"

namespace Gamnet { namespace Test {

boost::asio::io_service& GetIOService();
void CreateThreadPool(int threadCount);

class Session : public Network::Tcp::Session 
{
public:
	Session();
	virtual ~Session();

	int test_seq;
	uint32_t server_session_key;
	std::string server_session_token;
	bool is_pause;
	bool is_connected;
	std::chrono::time_point<std::chrono::steady_clock> send_time;
	std::function<void(const std::shared_ptr<Session>&)> execute_send_handler;
	Timer timer;

	virtual void OnCreate() override {}
	virtual void OnAccept() override {}
	virtual void OnConnect() {}
	virtual void OnClose(int reason) override {}

	virtual bool Init() override;
	virtual void Clear() override;
	void Pause(int millisecond);
	void Resume();
	void Next();
};

}} /* namespace Gamnet */

#endif /* TESTSESSION_H_ */
