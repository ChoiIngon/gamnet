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
			gold_amount.text = "0";
		}
		private void OnDestroy()
		{
			Util.EventSystem.Unsubscribe(Component.Counter.Event.SetCounter, OnUpdate);
		}
		private void OnEnable()
		{
			gold_amount.text = GameManager.Instance.counter.GetCount(Message.CounterType.Gold).ToString();
		}
		private void OnUpdate()
		{
			gold_amount.text = GameManager.Instance.counter.GetCount(Message.CounterType.Gold).ToString();
		}
	}
}
