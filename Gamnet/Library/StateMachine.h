#ifndef _GAMNET_LIB_STATE_MACHINE_H_
#define _GAMNET_LIB_STATE_MACHINE_H_

namespace Gamnet { 
	
	template <class STATE_T>
	class StateMachine
	{
	private :
		STATE_T init_state;
	public:
		STATE_T previous_state;
		STATE_T current_state;
		std::map<STATE_T, std::map<STATE_T, Gamnet::Delegate<void()>>> transition_callbacks;

		StateMachine()
		{
		}

		void Init(const STATE_T& init)
		{
			init_state = previous_state = current_state = init;
		}

		void AddTransitionCallback(const STATE_T& from, const STATE_T& to, std::function<void()> func)
		{
			auto& transition_callback = transition_callbacks[from];
			transition_callback[to] += func;
		}

		bool ChangeState(const STATE_T& next)
		{
			auto itr = transition_callbacks.find(current_state);
			if (transition_callbacks.end() == itr)
			{
				return false;
			}

			auto& transition_callback = itr->second;
			auto callback_itr = transition_callback.find(next);
			if (transition_callback.end() == callback_itr)
			{
				return false;
			}

			auto& callback_delegate = callback_itr->second;
			callback_delegate();
			previous_state = current_state;
			current_state = next;
			return true;
		}
	};
}
#endif
