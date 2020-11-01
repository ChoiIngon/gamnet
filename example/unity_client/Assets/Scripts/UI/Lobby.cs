using UnityEngine;
using UnityEngine.UI;

namespace UI
{
    public class Lobby : MonoBehaviour
    {
		public Button enter_button;
		private void Awake()
		{
			enter_button.onClick.AddListener(() => {
				GameManager.Instance.dungeon.Send_CreateDungeon_Req();
				GameManager.Instance.lobby.gameObject.SetActive(false);
				GameManager.Instance.ui.lobby.gameObject.SetActive(false);
				Camera.main.enabled = false;
			});
		}

		private void OnEnable()
		{
			enter_button.gameObject.SetActive(false);
			Send_Join_Req();
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
				GameManager.Instance.ui.alert.Open("MsgSvrCli_Join_Ans", ans.error_code.ToString() + "(" + ans.error_code.ToString() + ")");
				return;
			}
			enter_button.gameObject.SetActive(true);
		}
	}
}