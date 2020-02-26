using System.Collections;
using UnityEngine;
using UnityEngine.UI;
using UnityEditor;
using Message;

public class UnityClient : MonoBehaviour
{
	public InputField input_host;
	public InputField input_port;
	public Button button_connect;
	public Button button_send;
	public Button button_pause;
	public Button button_close;
	public ScrollRect scrollrect;
	public Text text_log;
	public bool run_in_background;

	private Gamnet.Session session = new Gamnet.Session();
	private Coroutine coroutine = null;
	private bool pause_toggle = false;
		
	private string host;
	private int port;
    private int line_count = 0;
    private const int LINE_LIMIT = 1000;
	private const int TimeoutError = 1000;

	private void Awake()
	{
		if (true == Application.isEditor)
		{
			Application.runInBackground = run_in_background;
		}
	}
	private void Start ()
	{
		input_host.text = PlayerPrefs.GetString("host");
		input_port.text = PlayerPrefs.GetInt("port").ToString();

		session.RegisterHandler<Message.MsgSvrCli_SendMessage_Ntf>((MsgSvrCli_SendMessage_Ntf ntf) => {
			Log("RECV MsgSvrCli_SendMessage_Ntf(message:" + ntf.Message + ")");
		});

		button_connect.onClick.AddListener(() => 
		{
			host = input_host.text;
			port = int.Parse(input_port.text);
			PlayerPrefs.SetString("host", host);
			PlayerPrefs.SetInt("port", port);
            session.Connect(host, port, 60000);
        });

		button_send.onClick.AddListener(() =>
		{
			Message.MsgCliSvr_SendMessage_Ntf ntf = new Message.MsgCliSvr_SendMessage_Ntf();
			ntf.Message = "Hello World";
			session.SendMsg(ntf, true);
		});

		button_pause.onClick.AddListener(() =>	
		{
 			if (Gamnet.Session.ConnectionState.Connected == session.state)
			{
				if (null != coroutine)
				{
					StopCoroutine(coroutine);
				}
				coroutine = null;
                session.Pause();
            }
			else
			{
				coroutine = StartCoroutine(SendHeartBeat());
			}
		});
		button_close.onClick.AddListener(() => {
			session.Close();
		});

        session.onConnect += () => {
			Log("connect success to " + host + ":" + port);
			button_pause.transform.Find("Text").GetComponent<Text>().text = "Pause";
		};
		session.onResume += () => {
            Log("resume");
			button_pause.transform.Find("Text").GetComponent<Text>().text = "Pause";
		};
        session.onClose += () => {
			button_pause.transform.Find("Text").GetComponent<Text>().text = "Resume";
			Log("close");
        };
        session.onError += (Gamnet.Exception e) => {
			Log(e.ToString());
			if(TimeoutError == e.ErrorCode)
			{
				return;
			}
			if(null != coroutine)
			{
				StopCoroutine(coroutine);
			}
			coroutine = null;
		};
	}

	IEnumerator SendHeartBeat() {
		/*
		while (true) {
			
			//if (Gamnet.Session.ConnectionState.Connected == session.state)
			{
				MsgCliSvr_HeartBeat_Ntf ntf = new MsgCliSvr_HeartBeat_Ntf();
				ntf.msg_seq = msg_seq++;
				Log ("MsgCliSvr_HeartBeat_Ntf(msg_seq:" + ntf.msg_seq + ")");
				session.SendMsg (ntf, true);			
			}

			yield return new WaitForSeconds(1.0f);

		}
		*/
		yield break;
	}
	// Update is called once per frame
	void Update () {
		session.Update ();
	}
		
    void Log(string text)
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

	private void OnApplicationPause(bool pause)
    {
        Log("UnityClient.OnApplicationPause(pause:" + pause.ToString() + ")");
        if (true == pause)
        {
            session.Pause();
        }
        else
        {
            session.Resume();
        }
    }
    private void OnApplicationQuit()
    {
		session.Close ();   
    }

	[MenuItem("IDL/Build")]
	private static void BuildIDL()
	{
		string idlc_path = System.IO.Path.Combine(Application.dataPath, "../../../x64/Debug");
		string message_path = System.IO.Path.Combine(Application.dataPath, "../../idl");
		
		if (Application.platform == RuntimePlatform.WindowsEditor)
		{
			System.Diagnostics.Process.Start(idlc_path + "/idlc", "/K -lcs " + message_path + "/Message.idl");
			System.IO.File.Copy(message_path + "/Message.cs", Application.dataPath + "/Scripts/IDL/Message.cs", true);
		}
		/*
		else if(Application.platform == RuntimePlatform.OSXEditor)
		{
			Process.Start(Application.dataPath + "/Script/Network/IDL/idlc", "/K -lcs " + Application.dataPath + "/Script/Network/IDL/idl");
			Process.Start(Application.dataPath + "/Script/Network/IDL/idlc", "/K -lcs " + Application.dataPath + "/Script/Network/IDL/XXError.idl");
			Process.Start(Application.dataPath + "/Script/Network/IDL/idlc", "/K -lcs " + Application.dataPath + "/Script/Network/IDL/XXMessage.idl");
			Process.Start(Application.dataPath + "/Script/Network/IDL/idlc", "/K -lcs " + Application.dataPath + "/Script/Network/IDL/P2PMessage.idl");
		}
        */
	}
}
