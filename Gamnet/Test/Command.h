#ifndef GAMNET_TEST_COMMAND_H_
#define GAMNET_TEST_COMMAND_H_

#include <functional>
#include <memory>
#include <string>

namespace Gamnet {	namespace Test {
	class Command {
	public :
		const std::string& name;

		Command(const std::string& name);
		virtual void Execute() = 0;
	};
	/*
	template<class SESSION_T>
	class MessageCommand : public Command 
	{
		typedef std::function<void(const std::shared_ptr<SESSION_T>&)> SEND_HANDLER_TYPE;
		typedef std::function<void(const std::shared_ptr<SESSION_T>&, const std::shared_ptr<Network::Tcp::Packet>&)> RECV_HANDLER_TYPE;
	public :
		MessageCommand(const std::string& name) : Command(name) 
		{
		}

		virtual void Execute() 
		{
		}
	};
	*/
}}
#endif

