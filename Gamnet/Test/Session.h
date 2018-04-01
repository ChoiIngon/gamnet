#ifndef GAMNET_TEST_SESSION_H_
#define GAMNET_TEST_SESSION_H_
#include "../Network/Network.h"

namespace Gamnet { namespace Test {


class Session : public Network::Tcp::Session {
public:
	int test_seq;
	bool is_pause;

	Session();
	virtual ~Session();

	virtual void OnCreate() override {}
	virtual void OnAccept() override {}
	virtual void OnConnect() {}
	virtual void OnClose(int reason) override {}

	bool Init() override;
	void Pause();
	void Resume();
};

}} /* namespace Gamnet */

#endif /* TESTSESSION_H_ */
