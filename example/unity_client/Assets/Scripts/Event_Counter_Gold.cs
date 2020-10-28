using UnityEngine;
using UnityEngine.UI;

class Event_Counter_Gold : MonoBehaviour
{
	public const string event_id = EventID.Event_OnUpdate_Counter_Gold;
	public Text gold = null;
	private void Start()
	{
		gold = GetComponent<Text>();
		Util.EventSystem.Subscribe<Message.CounterData>(event_id, OnUpdate);

		//Message.CounterData counter = GameManager.Instance.counter.GetCounter(Message.CounterType.Gold);
		//if (null == counter)
		//{
		//	gold.text = "0";
		//	return;
		//}
		//gold.text = counter.count.ToString();
	}

	private void OnUpdate(Message.CounterData counter)
	{
		gold.text = counter.count.ToString();
	}

	private void OnDesstroy()
	{
		Util.EventSystem.Unsubscribe<Message.CounterData>(event_id);
	}
}
