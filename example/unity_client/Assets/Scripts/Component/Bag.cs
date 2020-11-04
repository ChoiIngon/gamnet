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
			public const string UpdateItem = "Bag.Event.UpdateItem";
		}

		private Dictionary<UInt64, Item.Data> item_datas = new Dictionary<ulong, Item.Data>();

		public Bag()
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_AddItem_Ntf>(AddItem);
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_UpdateItem_Ntf>(UpdateItem);
		}

		~Bag()
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_AddItem_Ntf>(AddItem);
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_UpdateItem_Ntf>(UpdateItem);
		}

		public void AddItem(Message.Item.MsgSvrCli_AddItem_Ntf ntf)
		{
			foreach (Message.ItemData item in ntf.item_datas)
			{
				AddItem(item);
			}
		}

		public void AddItem(Message.ItemData data)
		{
			Debug.Log("item data:" + data.item_seq + ", item_count:" + data.item_count);

			Item.Data item = Item.Manager.Instance.CreateInstance(data);
			item_datas.Add(item.seq, item);
			Util.EventSystem.Publish<Item.Data>(Event.AddItem, item);
		}
		public void UpdateItem(Message.Item.MsgSvrCli_UpdateItem_Ntf ntf)
		{
			foreach (Message.ItemData item in ntf.item_datas)
			{
				AddItem(item);
			}
		}
		public void UpdateItem(Message.ItemData data)
		{
			if (0 == data.item_count)
			{
				RemoveItem(data.item_seq);
			}
			else
			{
				Item.Data item = GetItem(data.item_seq);
				item.count = data.item_count;
				Util.EventSystem.Publish<Item.Data>(Event.UpdateItem, item);
			}
		}

		private void RemoveItem(UInt64 itemSEQ)
		{
			item_datas.Remove(itemSEQ);
			Util.EventSystem.Publish<UInt64>(Event.RemoveItem, itemSEQ);
		}

		public Item.Data GetItem(UInt64 itemSEQ)
		{
			Item.Data data = null;
			if (false == item_datas.TryGetValue(itemSEQ, out data))
			{
				return null;
			}
			return data;
		}

		public void Clear()
		{
			item_datas = new Dictionary<ulong, Item.Data>();
		}

		public IEnumerator GetEnumerator()
		{
			foreach (var itr in item_datas)
			{
				Item.Data item = itr.Value;
				yield return item;
			}
		}

		public void Send_OpenPackage_Req(UInt64 itemSEQ)
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_OpenPackage_Ans>(Recv_OpenPackage_Ans);

			Message.Item.MsgCliSvr_OpenPackage_Req req = new Message.Item.MsgCliSvr_OpenPackage_Req();
			req.item_seq = itemSEQ;
			GameManager.Instance.LobbySession.SendMsg(req, true);
		}
		private void Recv_OpenPackage_Ans(Message.Item.MsgSvrCli_OpenPackage_Ans ans)
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_OpenPackage_Ans>(Recv_OpenPackage_Ans);
			if (0 == ans.item_data.item_count)
			{
				RemoveItem(ans.item_data.item_seq);
			}
			else
			{
				Item.Data item = GetItem(ans.item_data.item_seq);
				item.count = ans.item_data.item_count;
				Util.EventSystem.Publish<Item.Data>(Event.UpdateItem, item);
			}

		}
	}
}
