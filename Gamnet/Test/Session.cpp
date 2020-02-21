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

void Session::Clear()
{
	timer.Cancel();
	Network::Tcp::Session::Clear();
}

void Session::Pause(int millisecond)
{
	is_pause = true;
	timer.AutoReset(false);
	timer.SetTimer(millisecond, [=]() {
		link->strand.wrap(execute_send_handler)(std::static_pointer_cast<Session>(shared_from_this()));
	}); 
}

void Session::Resume()
{
	is_pause = false;
}

void Session::Next()
{
	test_seq++;
	//LOG(DEV, "session_key:", session_key, ", link_key:", link->link_key, ", test_seq:", test_seq);
	is_pause = false;
	while (0 < send_packets.size())
	{
		send_packets.pop_front();
	}
}
}}/* namespace Gamnet */
