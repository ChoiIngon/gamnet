using UnityEngine;
using System.Collections.Generic;

namespace Handler
{
	namespace Dungeon
	{
		class Handler_CreateDungeon
		{
			public static void SendMsg()
			{
				GameManager.Instance.session.RegisterHandler<Message.Dungeon.MsgSvrCli_CreateDungeon_Ans>(Handler_CreateDungeon.OnRecv);

				Message.Dungeon.MsgCliSvr_CreateDungeon_Req req = new Message.Dungeon.MsgCliSvr_CreateDungeon_Req();
				GameManager.Instance.session.SendMsg(req, true);
			}

			public static void OnRecv(Message.Dungeon.MsgSvrCli_CreateDungeon_Ans ans)
			{
				GameManager.Instance.session.UnregisterHandler<Message.Dungeon.MsgSvrCli_CreateDungeon_Ans>(Handler_CreateDungeon.OnRecv);
				GameObject go = new GameObject();
				go.name = "Dungeon";
				go.transform.SetParent(GameManager.Instance.scenes.dungeon_main.transform, false);

				Component.Dungeon dungeon = go.AddComponent<Component.Dungeon>();
				dungeon.width = ans.width;
				dungeon.height = ans.height;

				List<Tile.Data> tileDatas = new List<Tile.Data>(new Tile.Data[dungeon.width * dungeon.height]);
				for (int y = 0; y < dungeon.height; y++)
				{
					for (int x = 0; x < dungeon.width; x++)
					{
						if (Message.DungeonTileType.Invalid == ans.tiles[y * dungeon.width + x])
						{
							continue;
						}

						GameObject obj = new GameObject();
						obj.transform.SetParent(dungeon.transform, false);
						obj.transform.localPosition = new Vector3(x, y, 0.0f);
						//obj.SetActive(false);

						Tile tile = obj.AddComponent<Tile>();

						Tile.Data tileData = new Tile.Data();
						tileData.index = y * dungeon.width + x;
						tileData.type = ans.tiles[tileData.index];
						tileData.position = new Vector2Int(x, y);

						tile.Init(tileData);
						dungeon.tiles.Add(tile);
					}
				}

				GameManager.Instance.scenes.dungeon_main.dungeon = dungeon;
			}
		}
	}
}
