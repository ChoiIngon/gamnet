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

		// Start is called before the first frame update
		private void Awake()
		{
			input_host.gameObject.SetActive(false);
			input_port.gameObject.SetActive(false);
			button_connect.gameObject.SetActive(false);
			button_close.gameObject.SetActive(false);
			button_start.gameObject.SetActive(false);

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
				GameManager.Instance.ui.login.gameObject.SetActive(false);
				GameManager.Instance.ui.lobby.gameObject.SetActive(true);
				GameManager.Instance.ui.hud.gameObject.SetActive(true);
			});

			GameManager.Instance.LobbySession.onConnect += OnConnect;
			GameManager.Instance.LobbySession.onError += OnError;

			GameManager.Instance.LobbySession.RegisterHandler<Message.User.MsgSvrCli_Login_Ans>(Recv_Login_Ans);
			GameManager.Instance.LobbySession.RegisterHandler<Message.User.MsgSvrCli_Create_Ans>(Recv_CreateUser_Ans);
		}
		private void OnDestroy()
		{
			GameManager.Instance.LobbySession.onConnect -= OnConnect;
			GameManager.Instance.LobbySession.onError -= OnError;

			GameManager.Instance.LobbySession.UnregisterHandler<Message.User.MsgSvrCli_Login_Ans>(Recv_Login_Ans);
			GameManager.Instance.LobbySession.UnregisterHandler<Message.User.MsgSvrCli_Create_Ans>(Recv_CreateUser_Ans);
		}
		void Start()
		{
			input_host.text = PlayerPrefs.GetString("host");
			input_port.text = PlayerPrefs.GetInt("port").ToString();

			string host = PlayerPrefs.GetString("host");
			int port = PlayerPrefs.GetInt("port");

			if ("" != host && 0 != port)
			{
				GameManager.Instance.LobbySession.Connect(host, port);
				return;
			}

			input_host.gameObject.SetActive(true);
			input_port.gameObject.SetActive(true);
			button_connect.gameObject.SetActive(true);
			button_close.gameObject.SetActive(true);
		}

		private void OnConnect()
		{
			Send_Login_Req();
		}

		private void OnConnectFail()
		{
			GameManager.Instance.ui.alert.Open("Connect Fail", "Can not connect to host(" + input_host.text + ":" + input_port.text + ")");
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

		public void Send_Login_Req()
		{
			Message.User.MsgCliSvr_Login_Req req = new Message.User.MsgCliSvr_Login_Req();
			req.account_id = SystemInfo.deviceUniqueIdentifier;
			req.account_type = Message.AccountType.Dev;
			GameManager.Instance.LobbySession.SendMsg(req, true).SetTimeout(Message.User.MsgSvrCli_Login_Ans.MSG_ID, 5, () =>
			{

			});
		}
		public void Recv_Login_Ans(Message.User.MsgSvrCli_Login_Ans ans)
		{
			if (Message.ErrorCode.InvalidUserError == ans.error_code)
			{
				Send_CreateUser_Req();
				return;
			}
			button_start.gameObject.SetActive(true);
		}
		public void Send_CreateUser_Req()
		{
			Message.User.MsgCliSvr_Create_Req req = new Message.User.MsgCliSvr_Create_Req();
			req.account_id = SystemInfo.deviceUniqueIdentifier;
			req.account_type = Message.AccountType.Dev;
			GameManager.Instance.LobbySession.SendMsg(req, true);
		}
		public void Recv_CreateUser_Ans(Message.User.MsgSvrCli_Create_Ans ans)
		{
			if (Message.ErrorCode.Success != ans.error_code)
			{
				GameManager.Instance.LobbySession.Close();
				return;
			}
			Send_Login_Req();
		}
	}
}
