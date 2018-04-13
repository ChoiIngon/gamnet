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
	Log::Write(GAMNET_INF, "Gamnet server starts..");
	for (int i = 0; i<threadCount; i++)
	{
		workers_.push_back(std::thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
	}
}

Session::Session() : test_seq(-1), is_pause(false), Network::Tcp::Session(io_service_) {
}

Session::~Session() {
}

bool Session::Init()
{
	if(false == Network::Tcp::Session::Init())
	{
		return false;
	}
	test_seq = 0;
	is_pause = false;

	access_token = "";
	server_session_key = 0;
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
