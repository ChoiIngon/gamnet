using UnityEngine;

namespace Handler
{
	namespace User
	{
		class Handler_Create
		{
			public static void SendMsg()
			{
				//GameManager.Instance.session.RegisterHandler<Message.User.MsgSvrCli_Create_Ans>(Handler_Create.OnRecv);
				//
				//Message.User.MsgCliSvr_Create_Req req = new Message.User.MsgCliSvr_Create_Req();
				//req.account_id = SystemInfo.deviceUniqueIdentifier;
				//req.account_type = Message.AccountType.Dev;
				//GameManager.Instance.session.SendMsg(req, true);
			}

			public static void OnRecv(Message.User.MsgSvrCli_Create_Ans ans)
			{

				//GameManager.Instance.session.UnregisterHandler<Message.User.MsgSvrCli_Create_Ans>(Handler_Create.OnRecv);
				//
				//if (Message.ErrorCode.Success != ans.error_code)
				//{
				//	GameManager.Instance.session.Close();
				//	return;
				//}
				//
				//Handler_Login.SendMsg();
			}
		}
	}
}
