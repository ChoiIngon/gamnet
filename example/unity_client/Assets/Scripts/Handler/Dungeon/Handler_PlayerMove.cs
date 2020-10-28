using UnityEngine;
using System.Collections.Generic;

namespace Handler
{
	namespace Dungeon
	{
		class Handler_PlayerMove
		{
			public static void SendMsg(Vector2Int dest)
			{
				//GameManager.Instance.session.RegisterHandler<Message.Dungeon.MsgSvrCli_PlayerMove_Ans>(Handler_PlayerMove.OnRecv);
				//
				//Message.Dungeon.MsgCliSvr_PlayerMove_Req req = new Message.Dungeon.MsgCliSvr_PlayerMove_Req();
				//req.destination.x = dest.x;
				//req.destination.y = dest.y;
				//GameManager.Instance.session.SendMsg(req, true);
			}

			public static void OnRecv(Message.Dungeon.MsgSvrCli_PlayerMove_Ans ans)
			{
				/*
				GameManager.Instance.session.UnregisterHandler<Message.Dungeon.MsgSvrCli_PlayerMove_Ans>(Handler_PlayerMove.OnRecv);

				{
					List<Vector2Int> path = new List<Vector2Int>();
					foreach (Message.Vector2Int point in ans.path)
					{
						path.Add(new Vector2Int(point.x, point.y));
					}

					Unit player = GameManager.Instance.scenes.dungeon.player;
					player.SetMovePath(path);
				}

				foreach (var itr in ans.monster_moves)
				{
					List<Vector2Int> path = new List<Vector2Int>();
					foreach (Message.Vector2Int point in itr.Value)
					{
						path.Add(new Vector2Int(point.x, point.y));
					}

					Unit monster = GameManager.Instance.scenes.dungeon.monsters[itr.Key];
					monster.SetMovePath(path);
				}
				*/
			}
		}
	}
}
