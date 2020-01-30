#ifndef GAMNET_TEST_SESSION_H_
#define GAMNET_TEST_SESSION_H_
#include "../Network/Network.h"

namespace Gamnet { namespace Test {

boost::asio::io_service& GetIOService();
void CreateThreadPool(int threadCount);

class Session : public Network::Tcp::Session {
public:
	int repeat_count;
	uint32_t server_session_key;
	std::string server_session_token;
	int test_seq;
	bool is_pause;
	bool is_connected;
	std::chrono::time_point<std::chrono::steady_clock> send_time;
	Timer timer;

	Session();
	virtual ~Session();

	virtual void OnCreate() override {}
	virtual void OnAccept() override {}
	virtual void OnConnect() {}
	virtual void OnClose(int reason) override {}

	bool Init() override;
	void Pause(int millisecond);
	void Resume();
	void Next();
};

}} /* namespace Gamnet */

#endif /* TESTSESSION_H_ */
