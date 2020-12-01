using System;
using System.Collections.Generic;
using UnityEngine;

namespace Component
{
	public class Dungeon : MonoBehaviour
	{
		public Transform tile_root;
		public List<Tile> tiles = new List<Tile>();
		public int width = 0;
		public int height = 0;
		public Unit player;
		public Dictionary<UInt64, Unit> units = new Dictionary<UInt64, Unit>();
		private void Start()
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.Dungeon.MsgSvrCli_UnitPosition_Ntf>(Recv_UnitPosition_Ntf);
		}
		private void OnDestroy()
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Dungeon.MsgSvrCli_UnitPosition_Ntf>(Recv_UnitPosition_Ntf);
		}
		public void EnableTouch(bool flag)
		{
			foreach (Tile tile in tiles)
			{
				if (true == flag)
				{
					tile.touchInput.block_count--;
				}
				else
				{
					tile.touchInput.block_count++;
				}
			}
		}

		public Tile.Data GetTileData(int tileIndex)
		{
			if (0 > tileIndex || width * height <= tileIndex)
			{
				return null;
			}
			
			if (null == tiles[tileIndex])
			{
				return null;
			}
			return tiles[tileIndex].data;
		}

		public Tile.Data GetTileData(int x, int y)
		{
			return GetTileData(y * width + x);
		}

		void OnDrawGizmosSelected()
		{
			/*
			foreach (Block block in data.blocks)
			{
				Gizmos.DrawLine(new Vector3((float)block.rect.x - 0.5f, (float)block.rect.y - 0.5f, 0.0f), new Vector3((float)block.rect.x - 0.5f, (float)block.rect.yMax - 0.5f, 0.0f));
				Gizmos.DrawLine(new Vector3((float)block.rect.x - 0.5f, (float)block.rect.y - 0.5f, 0.0f), new Vector3((float)block.rect.xMax - 0.5f, (float)block.rect.y - 0.5f, 0.0f));
				Gizmos.DrawLine(new Vector3((float)block.rect.x - 0.5f, (float)block.rect.yMax - 0.5f, 0.0f), new Vector3((float)block.rect.xMax - 0.5f, (float)block.rect.yMax - 0.5f, 0.0f));
				Gizmos.DrawLine(new Vector3((float)block.rect.xMax - 0.5f, (float)block.rect.y - 0.5f, 0.0f), new Vector3((float)block.rect.xMax - 0.5f, (float)block.rect.yMax - 0.5f, 0.0f));
			}
			*/
		}

		public void Send_CreateDungeon_Req()
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.Dungeon.MsgSvrCli_CreateDungeon_Ans>(Recv_CreateDungeon_Ans);

			Message.Dungeon.MsgCliSvr_CreateDungeon_Req req = new Message.Dungeon.MsgCliSvr_CreateDungeon_Req();
			GameManager.Instance.LobbySession.SendMsg(req, true).SetTimeout(Message.Dungeon.MsgSvrCli_CreateDungeon_Ans.MSG_ID, 1, () =>
			{
			});
		}
		
		public void SetFieldOfView(Vector2Int position, int sight, bool visible)
		{
			BresenhamCircle2D circle = new BresenhamCircle2D(position, sight);

			foreach (Vector2Int circumference in circle)
			{
				BresenhamLine2D line = new BresenhamLine2D(position, circumference);
				foreach (Vector2Int point in line)
				{
					Tile.Data tile = GetTileData(point.x, point.y);
					if (null == tile)
					{
						break;
					}
					tile.visible = visible;

					if (Message.DungeonTileType.Wall == tile.type)
					{
						break;
					}

					if (width * height > tile.index + 1)
					{
						GetTileData(point.x + 1, point.y).visible = visible;
					}

					if (width * height > tile.index + width)
					{
						GetTileData(point.x, point.y + 1).visible = visible;
					}

					if (0 <= tile.index - 1)
					{
						GetTileData(point.x - 1, point.y).visible = visible;
					}

					if (0 <= tile.index - width)
					{
						GetTileData(point.x, point.y - 1).visible = visible;
					}
				}
			}
		}

		private void Recv_UnitPosition_Ntf(Message.Dungeon.MsgSvrCli_UnitPosition_Ntf ntf)
		{
			Unit unit = null; ;
			if (false == units.TryGetValue(ntf.unit_seq, out unit))
			{
				return;
			}

			unit.position = new Vector2Int(ntf.position.x, ntf.position.y);
		}
		private void Recv_CreateDungeon_Ans(Message.Dungeon.MsgSvrCli_CreateDungeon_Ans ans)
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Dungeon.MsgSvrCli_CreateDungeon_Ans>(Recv_CreateDungeon_Ans);
			if (Message.ErrorCode.Success != ans.error_code)
			{
				GameManager.Instance.ui.alert.Open("MsgSvrCli_CreateDungeon_Ans", ans.error_code.ToString() + "(" + ans.error_code.ToString() + ")");
				return;
			}

			this.width = ans.width;
			this.height = ans.height;
			this.tiles = new List<Tile>(new Tile[width * height]);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (Message.DungeonTileType.Invalid == ans.tiles[y * width + x])
					{
						continue;
					}

					Tile.Data tileData = new Tile.Data();
					tileData.index = y * width + x;
					tileData.type = ans.tiles[tileData.index];
					tileData.position = new Vector2Int(x, y);

					GameObject obj = new GameObject();
					obj.transform.SetParent(tile_root, false);
					obj.transform.localPosition = new Vector3(x, y, 0.0f);
					obj.SetActive(false);

					Tile tile = obj.AddComponent<Tile>();
					tile.Init(tileData);
					tiles[y * width + x] = tile;
				}
			}

			player.seq = ans.player_unit_seq;
			player.position = new Vector2Int(ans.start.x, ans.start.y);
			units.Add(player.seq, player);

			/*
			foreach (var monster in ans.monsters)
			{
				GameObject obj = new GameObject();
				obj.name = "Monster_" + monster.seq;
				Unit unit = obj.AddComponent<Unit>();
				unit.seq = monster.seq;
				unit.position = new Vector2Int(monster.position.x, monster.position.y);
				//obj.transform.SetParent(GameManager.Instance.scenes.dungeon.transform);
				obj.AddComponent<Monster>();
				//GameManager.Instance.scenes.dungeon.monsters[unit.seq] = unit;
			}
			*/

			gameObject.SetActive(true);
		}

	}

}