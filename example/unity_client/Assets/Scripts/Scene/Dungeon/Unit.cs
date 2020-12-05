using UnityEngine;
using System.Collections.Generic;
using System;

public class Unit : MonoBehaviour
{
	public Component.Dungeon dungeon;
	public UInt64 seq;
	public int max_health;
	public int cur_health;
	public int attack;
	public int defense;
	public int speed;
	public int sight;
	public float critical_chance;
	public float critical_rate;

	public Vector2Int position
	{
		get
		{
			return new Vector2Int((int)transform.position.x, (int)transform.position.y);
		}
		set
		{
			if (true == GetComponent<PlayerController>().enabled)
			{
				GameManager.Instance.dungeon.SetFieldOfView(position, sight, false);
				GameManager.Instance.dungeon.SetFieldOfView(value, sight, true);
			}
			else
			{
				Tile tile = null;
				tile = GameManager.Instance.dungeon.GetTileData(position.x, position.y);
				if (null != tile)
				{
					tile.Leave(this);
				}
				tile = GameManager.Instance.dungeon.GetTileData(value.x, value.y);
				if (null != tile)
				{
					tile.Enter(this);
				}
			}
			transform.position = new Vector3(value.x, value.y, 0.0f);
		}
	}

	public System.Action<float> onDamage;

	public void SetMovePath(List<Vector2Int> path)
	{
		this.path = path;
		this.delta_time = 1.0f;
	}

	private List<Vector2Int> path = null;
	private float delta_time = 0.0f;

	private void Update()
	{
		if (null != path)
		{
			if (0 >= path.Count)
			{
				path = null;
				return;
			}

			delta_time += Time.deltaTime;
			if (GameManager.TickTime > delta_time)
			{
				return;
			}

			delta_time = 0.0f;
			position = path[0];
			path.RemoveAt(0);
		}
	}

	public class Meta : MetaData
	{
		public string id;
		public UInt32 index;

		public Meta()
		{
		}
	}

	public class Manager : Util.Singleton<Manager>
	{
		public Manager()
		{
			//GameManager.Instance.LobbySession.RegisterHandler<Message.Dungeon.MsgSvrCli_UnitCreate_Ntf>(OnUnitCreate);
			//GameManager.Instance.LobbySession.RegisterHandler<Message.Dungeon.MsgSvrCli_UnitPosition_Ntf>(OnUnitPosition);
		}

		public void Init()
		{
			InitMeta("Assets/MetaData/Unit_Monster.csv");
		}
		private void InitMeta(string filePath)
		{
			TextAsset textAsset = AssetBundleManager.Instance.LoadAsset<TextAsset>(filePath);
			if (null == textAsset)
			{
				GameManager.Instance.ui.alert.Open("Failed to load meta file", "can not find '" + filePath + "'");
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

		private Dictionary<UInt32, Meta> index_metas = new Dictionary<UInt32, Meta>();
		private Dictionary<string, Meta> id_metas = new Dictionary<string, Meta>();
	}
}
