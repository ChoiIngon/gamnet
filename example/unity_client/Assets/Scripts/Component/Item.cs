using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Item
{
	public class Meta : MetaData
	{
		public enum ItemType
		{
			Invalid,
			Equip,
			Package
		}
		public class Equip : MetaData
		{
			public Equip()
			{
				CustomBind("part", OnPartType);
			}

			public Message.EquipItemPartType part;
			public int attack;
			public int defense;
			public int speed;
			public string sprite_path;
			void OnPartType(ref object member, string value)
			{
				member = Enum.Parse(typeof(Message.EquipItemPartType), (value));
			}
		}
		public class Price : MetaData
		{
			public Price()
			{
				CustomBind("type", OnPriceType);
			}
			void OnPriceType(ref object member, string value)
			{
				member = Enum.Parse(typeof(Message.CounterType), (value));
			}

			public Message.CounterType type = Message.CounterType.Invalid;
			public int value = 0;
		};
		public class Expire : MetaData
		{
			public enum TriggerType
			{
				None = 0,
				OnCreate = 1,
				OnEquip = 2
			};
			public enum ExpireType
			{
				Infinite = 0,
				DueDate = 1,
				Period = 2
			};
			public Expire()
			{
				CustomBind("trigger_type", OnTriggerType);
				CustomBind("expire_type", OnExpireType);
			}
			void OnTriggerType(ref object member, string value)
			{
				member = TriggerType.None;

				if ("OnCreate" == value)
				{
					member = TriggerType.OnCreate;
					return;
				}
				else if ("OnEquip" == value)
				{
					member = TriggerType.OnEquip;
					return;
				}
				throw new System.Exception();
			}
			void OnExpireType(ref object member, string value)
			{
				member = Enum.Parse(typeof(ExpireType), value);
			}

			public TriggerType trigger_type = TriggerType.None;
			public ExpireType expire_type = ExpireType.Infinite;
			public Int64 time = 0;
			public DateTime date = DateTime.MinValue;
		};
		public class Package : MetaData
		{
			public Package()
			{
			}

			public string id = "";
			public int count = 0;
		};

		public Meta()
		{
			CustomBind("type", OnItemType);
		}
		public override void OnLoad()
		{
		}
		public Data CreateInstance()
		{
			Data data = new Data();
			data.meta = this;
			if (null != equip)
			{
				data.equip = new Data.Equip();
			}
			if (null != packages)
			{
				data.package = new Data.Package();
			}
			return data;
		}
		private void OnItemType(ref object member, string value)
		{
			member = (ItemType)Enum.Parse(typeof(ItemType), value);
		}

		public string id;
		public UInt32 index;
		public string name;
		public string icon_path;
		public ItemType type;
		public int grade;
		public int max_stack;
		public Equip equip;
		public Price price;
		public Expire expire;
		public List<Package> packages;
	}

	[Serializable]
	public class Data
	{
		[Serializable]
		public class Equip
		{
			public Message.EquipItemPartType part;
		}
		[Serializable]
		public class Package
		{
		}
		public UInt64 seq;
		public Meta meta;
		public Equip equip;
		public Package package;
		public DateTime expire_date;
		public int count;
	}

	public class Manager : Util.Singleton<Manager>
	{
		public IEnumerator Init()
		{
			yield return AssetBundleManager.Instance.LoadAssetBundle("MetaData");
			yield return AssetBundleManager.Instance.LoadAssetBundle("Sprites");

			InitMeta("Assets/MetaData/Item");
			InitMeta("Assets/MetaData/EquipItem");
		}

		private void InitMeta(string itemMeta)
		{
			TextAsset textAsset = AssetBundleManager.Instance.LoadAsset<TextAsset>(itemMeta);
			if (null == textAsset)
			{
				GameManager.Instance.ui.alert.Open("Failed to load meta file", "can not find '" + itemMeta + "'");
				throw new System.Exception();
			}
			CSVReader csvReader = new CSVReader();
			csvReader.ReadStream(textAsset.text);
			MetaData.Reader<Meta> metaReader = new MetaData.Reader<Meta>();
			metaReader.Read(csvReader);
			foreach (Meta meta in metaReader)
			{
				if (true == id_metas.ContainsKey(meta.id))
				{
					throw new System.Exception("duplicate item id(" + meta.id + ")");
				}

				if (true == index_metas.ContainsKey(meta.index))
				{
					throw new System.Exception("duplicate item index(" + meta.index + ")");
				}

				id_metas.Add(meta.id, meta);
				index_metas.Add(meta.index, meta);
			}
		}
		public Data CreateInstance(Message.ItemData data)
		{
			Meta meta = FindMeta(data.item_index);
			if (null == meta)
			{
				throw new System.Exception("can not find item(index:" + data.item_index + ")");
			}

			Data item = meta.CreateInstance();
			item.seq = data.item_seq;
			item.count = data.item_count;
			return item;
		}
		public Meta FindMeta(string id)
		{
			Meta meta = null;
			if (false == id_metas.TryGetValue(id, out meta))
			{
				return null;
			}
			return meta;
		}
		public Meta FindMeta(UInt32 index)
		{
			Meta meta = null;
			if (false == index_metas.TryGetValue(index, out meta))
			{
				return null;
			}
			return meta;
		}

		private Dictionary<UInt32, Meta> index_metas = new Dictionary<UInt32, Meta>();
		private Dictionary<string, Meta> id_metas = new Dictionary<string, Meta>();
	}	
}