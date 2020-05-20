#include "Executer.h"
#include "../Library/Exception.h"

namespace Gamnet { namespace Test {
	Command_Timer::Command_Timer(const std::string& id_, int time_) : id(id_), time(time_) {
	}

	void Command_Timer::Execute(const std::shared_ptr<Session>& session) {
		std::shared_ptr<Time::Timer> timer = Time::Timer::Create();
		timer->SetTimer(time, [=]() {
			Singleton<Executer>::GetInstance().OnCondition("OnTimer", {{ "id", this->id }}, session);
		});
	}

	void Executer::OnCondition(const std::string& name, const std::map<std::string, boost::any>& parameter, const std::shared_ptr<Session>& session)
	{
		const std::string conditionKey = MakeKey(name, parameter);
		auto itr = command_lists.find(conditionKey);
		if (command_lists.end() == itr)
		{
			throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "can not find condition(condition_key:", conditionKey, ")");
		}
		std::list<std::shared_ptr<Command>>& command_list = itr->second;
		for(auto command : command_list)
		{
			command->Execute(session);
		}
	}

	void Executer::AddCommand(const std::string& name, const std::map<std::string, boost::any>& parameter, const std::shared_ptr<Command>& command)
	{
		const std::string conditionKey = MakeKey(name, parameter);
		auto itr = command_lists.find(conditionKey);
		if(command_lists.end() == itr)
		{
			throw GAMNET_EXCEPTION(ErrorCode::UndefinedError, "can not find condition(condition_key:", conditionKey, ")");
		}
		std::list<std::shared_ptr<Command>>& command_list = itr->second;
		command_list.push_back(command);
	}
	std::string Executer::MakeKey(const std::string& name, const std::map<std::string, boost::any>& parameter)
	{
		std::string key = name + ";";
		for (auto& itr : parameter)
		{
			key += itr.first + "=" + boost::any_cast<std::string>(itr.second) + ";";
		}
		return key;
	}
}}