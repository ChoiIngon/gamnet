using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Component
{
	public class Bag : IEnumerable
	{
		public class Event
		{
			public const string AddItem = "Bag.Event.AddItem";
			public const string RemoveItem = "Bag.Event.RemoveItem";
		}

		private Dictionary<UInt64, Message.ItemData> item_datas = new Dictionary<ulong, Message.ItemData>();

		public Bag()
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_Item_Ntf>(AddItem);
		}

		~Bag()
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_Item_Ntf>(AddItem);
		}

		public void AddItem(Message.Item.MsgSvrCli_Item_Ntf ntf)
		{
			foreach (Message.ItemData item in ntf.item_datas)
			{
				AddItem(item);
			}
		}

		public void AddItem(Message.ItemData data)
		{
			Debug.Log("item data:" + data.item_seq + ", item_count:" + data.item_count);
			item_datas.Add(data.item_seq, data);
			Util.EventSystem.Publish<Message.ItemData>(Event.AddItem, data);
		}

		public void RemoveItem(UInt64 itemSEQ)
		{
			item_datas.Remove(itemSEQ);
			Util.EventSystem.Publish<UInt64>(Event.RemoveItem, itemSEQ);
		}

		public Message.ItemData GetItem(UInt64 itemSEQ)
		{
			Message.ItemData data = null;
			if (false == item_datas.TryGetValue(itemSEQ, out data))
			{
				return null;
			}
			return data;
		}

		public void Clear()
		{
			item_datas = new Dictionary<ulong, Message.ItemData>();
		}

		public IEnumerator GetEnumerator()
		{
			foreach (var itr in item_datas)
			{
				Message.ItemData item = itr.Value;
				yield return item;
			}
		}
	}
}
