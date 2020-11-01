using UnityEngine;
using UnityEngine.UI;

namespace UI
{
	public class Gold : MonoBehaviour
	{
		private Text gold_amount;
		private void Awake()
		{
			Util.EventSystem.Subscribe(Component.Counter.Event.SetCounter, OnUpdate);
			gold_amount = transform.Find("Text").GetComponent<Text>();
			gold_amount.text = "LOAD ERROR";
		}
		private void OnDestroy()
		{
			Util.EventSystem.Unsubscribe(Component.Counter.Event.SetCounter, OnUpdate);
		}
		private void OnEnable()
		{
			Message.CounterData counter = GameManager.Instance.counter.GetCounter(Message.CounterType.Gold);
			gold_amount.text = counter.count.ToString();
		}
		private void OnUpdate()
		{
			Message.CounterData counter = GameManager.Instance.counter.GetCounter(Message.CounterType.Gold);
			gold_amount.text = counter.count.ToString();
		}
	}
}
