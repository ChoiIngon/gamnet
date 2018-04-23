#include "Session.h"
#include <thread>

namespace Gamnet { namespace Test {

static std::vector<std::thread > workers_;
static boost::asio::io_service& io_service_ = GetIOService();

boost::asio::io_service& GetIOService()
{
	static boost::asio::io_service _io_service;
	return _io_service;
}

void CreateThreadPool(int threadCount)
{
	LOG(GAMNET_INF, "[Test] test start...");
	for (int i = 0; i<threadCount; i++)
	{
		workers_.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
	}
}

Session::Session() : 
	Network::Tcp::Session(io_service_), 
	server_session_key(0),
	server_session_token(""),
	test_seq(-1), 
	is_pause(false),
	is_connected(false)
{
}

Session::~Session() {
}

bool Session::Init()
{
	if(false == Network::Tcp::Session::Init())
	{
		return false;
	}
	server_session_key = 0;
	server_session_token = "";
	test_seq = 0;
	is_pause = false;
	is_connected = false;

	return true;
}

void Session::Pause()
{
	is_pause = true;
}

void Session::Resume()
{
	is_pause = false;
}

}}/* namespace Gamnet */
