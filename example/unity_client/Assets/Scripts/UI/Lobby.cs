using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace UI
{
    public class Lobby : MonoBehaviour
    {
        // Start is called before the first frame update
        void Start()
        {

        }

		private void OnEnable()
		{
			//Send_Join_Req();
        }
		// Update is called once per frame
		void Update()
        {

        }

		public void Send_Join_Req()
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.Lobby.MsgSvrCli_Join_Ans>(Recv_Join_Ans);

			Message.Lobby.MsgCliSvr_Join_Req req = new Message.Lobby.MsgCliSvr_Join_Req();
			GameManager.Instance.LobbySession.SendMsg(req, true);
		}
		public void Recv_Join_Ans(Message.Lobby.MsgSvrCli_Join_Ans ans)
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.Lobby.MsgSvrCli_Join_Ans>(Recv_Join_Ans);
			if (Message.ErrorCode.Success != ans.error_code)
			{
				return;
			}
		}
	}
}