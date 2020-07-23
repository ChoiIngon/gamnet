#include "UserSession.h"
#include <boost/program_options.hpp>
#include <boost/coroutine2/all.hpp>

void OnRouterConnect(const Gamnet::Network::Router::Address& address)
{
	LOG(INF, "OnConnect:", address.ToString());
}

void OnRouterClose(const Gamnet::Network::Router::Address& address)
{
	LOG(DEV, "OnClose:", address.ToString());
}

class Coroutine
{
	boost::coroutines2::coroutine<void>::pull_type pull_type;
	boost::coroutines2::coroutine<void>::push_type* push_type;
public :
	Coroutine() : pull_type(std::bind(&Coroutine::Init, this, std::placeholders::_1)), push_type(nullptr)
	{
	}


	typedef void (Coroutine::*COROUTINE_FUNC_T)(const std::shared_ptr<Gamnet::Network::Tcp::Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet);

	COROUTINE_FUNC_T coroutine;
	std::shared_ptr<Gamnet::Network::Tcp::Session> session;
	std::shared_ptr<Gamnet::Network::Tcp::Packet> packet;
	
	void start()
	{
		pull_type();
	}
	void resume()
	{
		pull_type();
	}
protected:
	void yield()
	{
		std::cout << "yield" << std::endl;
		(*this->push_type)();
	}
private :
	void Init(boost::coroutines2::coroutine<void>::push_type& push)
	{
		push();
		push_type = &push;
		std::cout << "on async read complete" << std::endl;
		std::bind(coroutine, this, session, packet)();
	}
};



class CoroutineEx : public Coroutine
{
	Gamnet::Time::Timer timer;
public :
	void OnReceive(const std::shared_ptr<Gamnet::Network::Tcp::Session>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
	{
		std::cout << "de-serialize packet" << std::endl;
		std::cout << "send to back-end server" << std::endl;
		std::cout << "start async wait" << std::endl;
		timer.SetTimer(1000, [this] () {
			resume();
		});
		yield();
		std::cout << "on async read complete from backend server" << std::endl;
		std::cout << "logic process" << std::endl;
		std::cout << "response to client" << std::endl;
	}
};

int main(int argc, char** argv) 
{
	Coroutine* coro = new CoroutineEx();
	coro->coroutine = (Coroutine::COROUTINE_FUNC_T)&CoroutineEx::OnReceive;
	coro->start();

	boost::program_options::options_description desc("All Options");
	desc.add_options()
		("config", boost::program_options::value<std::string>()->default_value("config.xml"), "config file path")
		("thread", boost::program_options::value<int>()->default_value(std::thread::hardware_concurrency()), "working thread count")
		("help", "");

	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);

	if (0 != vm.count("help"))
	{
		std::cerr << desc << std::endl;
		return 1;
	}

	const char* config_path = vm["config"].as<std::string>().c_str();

	Gamnet::InitCrashDump();
	try {
		Gamnet::Log::ReadXml(config_path);
		LOG(INF, argv[0], " Server Starts..");
		LOG(INF, "build date:", __DATE__, " ", __TIME__);
		LOG(INF, "local ip:", Gamnet::Network::Tcp::GetLocalAddress().to_string());

		Gamnet::Network::Tcp::ReadXml<UserSession>(config_path);
		Gamnet::Network::Http::ReadXml(config_path);
		Gamnet::Network::Router::ReadXml(config_path, OnRouterConnect, OnRouterClose);
		Gamnet::Test::ReadXml<TestSession>(config_path);
		Gamnet::Run(vm["thread"].as<int>());
	}
	catch (const Gamnet::Exception& e)
	{
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what(), "(error_code:", e.error_code(), ")");
		return 1;
	}
	return 0;
}


