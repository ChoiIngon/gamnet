using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Component
{
    public class Suit : IEnumerable
	{
        public class Event
        {
            public const string EquipItem = "Suit.Event.EquipItem";
            public const string UnequipItem = "Suit.Event.UnequipItem";
        }

        private Item.Data[] item_datas = new Item.Data[(int)Message.EquipItemPartType.Max];

        public Suit()
        {
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_EquipItem_Ntf>(OnEquipItem);
			GameManager.Instance.LobbySession.RegisterHandler<Message.Item.MsgSvrCli_UnequipItem_Ntf>(OnUnequipItem);
		}
        ~Suit()
        {
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_EquipItem_Ntf>(OnEquipItem);
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Item.MsgSvrCli_UnequipItem_Ntf>(OnUnequipItem);
		}

		void OnEquipItem(Message.Item.MsgSvrCli_EquipItem_Ntf ntf)
		{
			Item.Data item = GameManager.Instance.bag.GetItem(ntf.item_seq);
			item.equip.part = item.meta.equip.part;
			item_datas[(int)item.equip.part] = item;
			Util.EventSystem.Publish<Item.Data>(Event.EquipItem, item);
		}

		void OnUnequipItem(Message.Item.MsgSvrCli_UnequipItem_Ntf ntf)
		{
			Item.Data item = item_datas[(int)ntf.part_type];
			item.equip.part = Message.EquipItemPartType.Invalid;
			item_datas[(int)ntf.part_type] = null;
			Util.EventSystem.Publish<Item.Data>(Event.UnequipItem, item);
		}

		public Item.Data GetItem(Message.EquipItemPartType part)
		{
			return item_datas[(int)part];
		}
		public IEnumerator GetEnumerator()
		{
			foreach (var itr in item_datas)
			{
				Item.Data item = itr;
				yield return item;
			}
		}
	}
}