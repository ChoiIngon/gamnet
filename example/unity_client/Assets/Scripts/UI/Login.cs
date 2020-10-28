using System;
using UnityEngine;
using UnityEngine.UI;

namespace UI
{
	public class Login : MonoBehaviour
	{
		public InputField input_host;
		public InputField input_port;
		public Button button_connect;
		public Button button_close;
		public ScrollRect scrollrect;
		public Text text_log;
		public Button button_start;

		private int line_count = 0;
		private const int LINE_LIMIT = 1000;
		private void Awake()
		{
		}

		// Start is called before the first frame update
		void Start()
		{
			input_host.gameObject.SetActive(false);
			input_port.gameObject.SetActive(false);
			button_connect.gameObject.SetActive(false);
			button_close.gameObject.SetActive(false);
			button_start.gameObject.SetActive(false);

			input_host.text = PlayerPrefs.GetString("host");
			input_port.text = PlayerPrefs.GetInt("port").ToString();

			GameManager.Instance.LobbySession.onConnect += OnConnect;
			GameManager.Instance.LobbySession.onError += OnError;

			button_connect.onClick.AddListener(() =>
			{
				input_host.gameObject.SetActive(false);
				input_port.gameObject.SetActive(false);
				button_connect.gameObject.SetActive(false);
				button_close.gameObject.SetActive(false);
				button_start.gameObject.SetActive(false);

				PlayerPrefs.SetString("host", input_host.text);
				PlayerPrefs.SetInt("port", int.Parse(input_port.text));
				GameManager.Instance.LobbySession.Connect(input_host.text, int.Parse(input_port.text));
			});

			button_start.onClick.AddListener(() =>
			{
				GameManager.Instance.ui.SetActive(UI.UIMain.UIKey_Login, false);
				GameManager.Instance.ui.SetActive(UI.UIMain.UIKey_Lobby, true);
			});

			string host = PlayerPrefs.GetString("host");
			int port = PlayerPrefs.GetInt("port");
			Log("Try to connect to host(" + host + ":" + port + ")");
			if ("" != host && 0 != port)
			{
				GameManager.Instance.LobbySession.Connect(host, port);
				return;
			}

			input_host.gameObject.SetActive(true);
			input_port.gameObject.SetActive(true);
			button_connect.gameObject.SetActive(true);
			button_close.gameObject.SetActive(true);
			//GameManager.Instance.session.RegisterHandler<Message.User.MsgSvrCli_Counter_Ntf>(Handler.User.Handler_Counter.OnRecv);
			//GameManager.Instance.session.RegisterHandler<Message.Item.MsgSvrCli_Item_Ntf>(Handler.Item.Handler_Item.OnRecv);
		}

		private void OnConnect()
		{
			Send_Login_Req();
		}

		private void OnConnectFail()
		{
			Log("Can not connect to host(" + input_host.text + ":" + input_port.text + ")");
			input_host.gameObject.SetActive(true);
			input_port.gameObject.SetActive(true);
			button_connect.gameObject.SetActive(true);
			button_close.gameObject.SetActive(true);
		}

		private void OnError(Gamnet.Exception e)
		{
			if (Gamnet.ErrorCode.ConnectFailError == e.ErrorCode)
			{
				OnConnectFail();
			}
			if (Gamnet.ErrorCode.ConnectTimeoutError == e.ErrorCode)
			{
			}
			if (Gamnet.ErrorCode.ReconnectFailError == e.ErrorCode)
			{
				// 초기화 화면
			}
			GameManager.Instance.LobbySession.Close();
		}

		private void Log(string text)
		{
			line_count++;
			text_log.text += line_count.ToString() + ":" + text + System.Environment.NewLine;
			if (LINE_LIMIT < line_count)
			{
				int index = text_log.text.IndexOf(System.Environment.NewLine);
				text_log.text = text_log.text.Substring(index + System.Environment.NewLine.Length);
			}
			scrollrect.verticalNormalizedPosition = 0.0f;
		}

		private void OnDestroy()
		{
			//GameManager.Instance.session.UnregisterHandler<Message.User.MsgSvrCli_Counter_Ntf>(Handler.User.Handler_Counter.OnRecv);
			//GameManager.Instance.session.UnregisterHandler<Message.Item.MsgSvrCli_Item_Ntf>(Handler.Item.Handler_Item.OnRecv);
			//GameManager.Instance.scenes.lobby = null;
		}


		public void Send_Login_Req()
		{
			GameManager.Instance.LobbySession.RegisterHandler<Message.User.MsgSvrCli_Login_Ans>(Recv_Login_Ans);

			Message.User.MsgCliSvr_Login_Req req = new Message.User.MsgCliSvr_Login_Req();
			req.account_id = SystemInfo.deviceUniqueIdentifier;
			req.account_type = Message.AccountType.Dev;
			GameManager.Instance.LobbySession.SendMsg(req, true);
		}
		public void Recv_Login_Ans(Message.User.MsgSvrCli_Login_Ans ans)
		{
			GameManager.Instance.LobbySession.UnregisterHandler<Message.User.MsgSvrCli_Login_Ans>(Recv_Login_Ans);
			if (Message.ErrorCode.InvalidUserError == ans.error_code)
			{
				Handler.User.Handler_Create.SendMsg();
				return;
			}
			button_start.gameObject.SetActive(true);
		}
	}
}
