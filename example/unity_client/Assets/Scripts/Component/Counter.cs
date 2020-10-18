using System.Collections;
using System.Collections.Generic;

namespace Component
{
	public class Counter : IEnumerable
	{
		public Message.CounterData SetCounter(Message.CounterData data)
		{
			Message.CounterData counter = GetCounter(data.counter_type);
			if (null == counter)
			{
				counters.Add(data.counter_type, data);
			}
			else
			{
				counters[data.counter_type] = data;
			}

			Util.EventSystem.Publish<Message.CounterData>(EventID.Event_OnUpdate_Counter_Gold, counter);
			return data;
		}

		public Message.CounterData GetCounter(Message.CounterType type)
		{
			Message.CounterData data = null;
			if (false == counters.TryGetValue(type, out data))
			{
				return null;
			}
			return data;
		}

		public IEnumerator GetEnumerator()
		{
			foreach (var itr in counters)
			{
				Message.CounterData counter = itr.Value;
				yield return counter;
			}
		}
		private Dictionary<Message.CounterType, Message.CounterData> counters = new Dictionary<Message.CounterType, Message.CounterData>();
	}
}
