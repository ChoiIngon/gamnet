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
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_AddItem_Ntf>(OnAddItem);
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_UpdateItem_Ntf>(OnUpdateItem);
		}

		~Bag()
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_AddItem_Ntf>(OnAddItem);
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_UpdateItem_Ntf>(OnUpdateItem);
		}

		public void OnAddItem(Message.Item.MsgSvrCli_AddItem_Ntf ntf)
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

		public void OnUpdateItem(Message.Item.MsgSvrCli_UpdateItem_Ntf ntf)
		{
			foreach (Message.ItemData item in ntf.item_datas)
			{
				UpdateItem(item);
			}
		}
		public void UpdateItem(Message.ItemData data)
		{
			if (0 == data.item_count)
			{
				RemoveItem(data.item_seq);
				return;
			}
			
			Item.Data item = GetItem(data.item_seq);
			item.count = data.item_count;
			Util.EventSystem.Publish<Item.Data>(Event.UpdateItem, item);
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
			if (Message.ErrorCode.Success != ans.error_code)
			{
				GameManager.Instance.ui.alert.Open("Recv_OpenPackage_Ans", ans.error_code.ToString() + "(" + ans.error_code.ToString() + ")");
				return;
			}
		}

		public void Send_EquipItem_Req(UInt64 itemSEQ)
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_EquipItem_Ans>(Recv_EquipItem_Ans);

			Message.Item.MsgCliSvr_EquipItem_Req req = new Message.Item.MsgCliSvr_EquipItem_Req();
			req.item_seq = itemSEQ;
			GameManager.Instance.LobbySession.SendMsg(req, true);
		}

		public void Recv_EquipItem_Ans(Message.Item.MsgSvrCli_EquipItem_Ans ans)
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_EquipItem_Ans>(Recv_EquipItem_Ans);
			if (Message.ErrorCode.Success != ans.error_code)
			{
				GameManager.Instance.ui.alert.Open("MsgSvrCli_EquipItem_Ans", ans.error_code.ToString() + "(" + ans.error_code.ToString() + ")");
				return;
			}
		}

		public void Send_UnequipItem_Req(Message.EquipItemPartType part)
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_UnequipItem_Ans>(Recv_UnequipItem_Ans);

			Message.Item.MsgCliSvr_UnequipItem_Req req = new Message.Item.MsgCliSvr_UnequipItem_Req();
			req.part_type = part;
			GameManager.Instance.LobbySession.SendMsg(req, true);
		}

		public void Recv_UnequipItem_Ans(Message.Item.MsgSvrCli_UnequipItem_Ans ans)
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_UnequipItem_Ans>(Recv_UnequipItem_Ans);
			if (Message.ErrorCode.Success != ans.error_code)
			{
				GameManager.Instance.ui.alert.Open("MsgSvrCli_UnequipItem_Ans", ans.error_code.ToString() + "(" + ans.error_code.ToString() + ")");
				return;
			}
		}

		public void Send_SellItem_Req(Item.Data item)
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_SellItem_Ans>(Recv_SellItem_Ans);

			Message.Item.MsgCliSvr_SellItem_Req req = new Message.Item.MsgCliSvr_SellItem_Req();
			req.item_seq = item.seq;
			req.item_count = item.count;
			GameManager.Instance.LobbySession.SendMsg(req, true);
		}

		public void Recv_SellItem_Ans(Message.Item.MsgSvrCli_SellItem_Ans ans)
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_SellItem_Ans>(Recv_SellItem_Ans);
			if (Message.ErrorCode.Success != ans.error_code)
			{
				GameManager.Instance.ui.alert.Open("MsgSvrCli_UnequipItem_Ans", ans.error_code.ToString() + "(" + ans.error_code.ToString() + ")");
				return;
			}
		}

	}
}
