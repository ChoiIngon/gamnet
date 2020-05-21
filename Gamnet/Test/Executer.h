#ifndef _GAMNET_TEST_EXECUTER_H_
#define _GAMNET_TEST_EXECUTER_H_

#include "Session.h"
#include "../Library/Singleton.h"
#include "../Library/Timer.h"

#include <boost/any.hpp>
#include <list>
#include <map>
#include <string>

namespace Gamnet { namespace Test {
	class Command
	{
	public :
		virtual void Execute(const std::shared_ptr<Session>& session) = 0;
	};
	
	
	template <class SESSION_T>
	class Command_SendMsg : public Command 
	{
		typedef std::function<void(const std::shared_ptr<SESSION_T>&)> SEND_FUNCTOR_T;
		SEND_FUNCTOR_T send_functor;
	public :
		Command_SendMsg(SEND_FUNCTOR_T& sendFunctor) : send_functor(sendFunctor)
		{
		}

		virtual void Execute(const std::shared_ptr<Session>& session) override {
			send_functor(std::static_pointer_cast<SESSION_T>(session));
		}
	};

	class Command_Timer : public Command
	{
		std::string id;
		int time;
	public :
		Command_Timer(const std::string& id, int time);
		virtual void Execute(const std::shared_ptr<Session>& session) override;
	};

	class Executer
	{
	private :
		std::map<std::string, std::list<std::shared_ptr<Command>>> command_lists;
	public :
		void OnCondition(const std::string& name, const std::map<std::string, boost::any>& parameter, const std::shared_ptr<Session>& session);

		void AddCommand(const std::string& name, const std::map<std::string, boost::any>& parameter, const std::shared_ptr<Command>& command);
	private :
		std::string MakeKey(const std::string& name, const std::map<std::string, boost::any>& parameter);
};
}}
#endif
