using UnityEngine.SceneManagement;

namespace Handler.Lobby
{
	class Handler_Join
	{	
		public static void SendMsg()
		{
			GameManager.Instance.session.RegisterHandler<Message.Lobby.MsgSvrCli_Join_Ans>(Handler_Join.OnRecv);

			Message.Lobby.MsgCliSvr_Join_Req req = new Message.Lobby.MsgCliSvr_Join_Req();
			GameManager.Instance.session.SendMsg(req, true);
		}
		public static void OnRecv(Message.Lobby.MsgSvrCli_Join_Ans ans)
		{
			GameManager.Instance.session.UnregisterHandler<Message.Lobby.MsgSvrCli_Join_Ans>(Handler_Join.OnRecv);

			if (Message.ErrorCode.Success != ans.error_code)
			{
				return;
			}

			SceneManager.LoadScene(Scene.Dungeon.Main.GetName());
		}
	}
}
