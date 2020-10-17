using UnityEngine;
using UnityEngine.SceneManagement;

namespace Handler
{
	namespace User
	{
		class Handler_Login
		{
			public static void SendMsg()
			{
				GameManager.Instance.session.RegisterHandler<Message.User.MsgSvrCli_Login_Ans>(Handler_Login.OnRecv);

				Message.User.MsgCliSvr_Login_Req req = new Message.User.MsgCliSvr_Login_Req();
				req.account_id = SystemInfo.deviceUniqueIdentifier;
				req.account_type = Message.AccountType.Dev;
				GameManager.Instance.session.SendMsg(req, true);
			}

			public static void OnRecv(Message.User.MsgSvrCli_Login_Ans ans)
			{
				GameManager.Instance.session.UnregisterHandler<Message.User.MsgSvrCli_Login_Ans>(Handler_Login.OnRecv);
				if (Message.ErrorCode.InvalidUserError == ans.error_code)
				{
					Handler.User.Handler_Create.SendMsg();
					return;
				}

				SceneManager.LoadScene(Scene.Dungeon.Main.GetName());
			}
		}
	}
}
