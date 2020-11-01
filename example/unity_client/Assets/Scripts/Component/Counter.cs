using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Component
{
	public class Counter : IEnumerable
	{
		public class Event
		{
			public const string SetCounter = "Component.Counter.Event.SetCounter";
		}
		public Counter()
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.User.MsgSvrCli_Counter_Ntf>(Recv_Counter_Ntf);
			Debug.Log("init counter");
		}

		~Counter()
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.User.MsgSvrCli_Counter_Ntf>(Recv_Counter_Ntf);
		}

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

			Util.EventSystem.Publish(Event.SetCounter);
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

		private void Recv_Counter_Ntf(Message.User.MsgSvrCli_Counter_Ntf ntf)
		{
			Debug.Log("successed to load counter");
			foreach (Message.CounterData counter in ntf.counter_datas)
			{
				SetCounter(counter);
			}
			Util.EventSystem.Publish(Event.SetCounter);
		}

		private Dictionary<Message.CounterType, Message.CounterData> counters = new Dictionary<Message.CounterType, Message.CounterData>();
	}
}
