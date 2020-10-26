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

		private int line_count = 0;
		private const int LINE_LIMIT = 1000;
		private void Awake()
		{
		}

		// Start is called before the first frame update
		void Start()
		{
			input_host.text = PlayerPrefs.GetString("host");
			input_port.text = PlayerPrefs.GetInt("port").ToString();

			button_connect.onClick.AddListener(() =>
			{
				string host = input_host.text;
				int port = int.Parse(input_port.text);
				PlayerPrefs.SetString("host", host);
				PlayerPrefs.SetInt("port", port);
				GameManager.Instance.session.Connect(host, port);
			});

			GameManager.Instance.session.onConnect += () =>
			{
				Log("success..connect");
				Handler.User.Handler_Login.SendMsg();
			};

			GameManager.Instance.session.RegisterHandler<Message.User.MsgSvrCli_Counter_Ntf>(Handler.User.Handler_Counter.OnRecv);
			GameManager.Instance.session.RegisterHandler<Message.Item.MsgSvrCli_Item_Ntf>(Handler.Item.Handler_Item.OnRecv);
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
			GameManager.Instance.session.UnregisterHandler<Message.User.MsgSvrCli_Counter_Ntf>(Handler.User.Handler_Counter.OnRecv);
			GameManager.Instance.session.UnregisterHandler<Message.Item.MsgSvrCli_Item_Ntf>(Handler.Item.Handler_Item.OnRecv);
			GameManager.Instance.scenes.lobby = null;
		}
	}
}
