using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using message;

public class UnityClient : MonoBehaviour {
	private Gamnet.Session session = new Gamnet.Session();
	private Coroutine coroutine = null;
	private UserData user_data = null;
	private bool pause_toggle = false;
    private uint msg_seq = 1;

	public Button connect;
	public Button close;
    public Button pause;
	public InputField host;
    public ScrollRect scrollRect;

    public Text logText;
    public int lineCount;
    public int lineNum;
    public int lineLimit = 1000;


	void Start () {
        connect.onClick.AddListener(() => {
			//session.msg_seq = 0;
            if ("" == host.text)
            {
                session.Connect("52.78.185.159", 20000, 60000);
            }
            else
            {
                session.Connect(host.text, 20000, 60000);
            }
        });
		pause.onClick.AddListener(() =>	{
 			if (false == pause_toggle) {
				pause.transform.Find("Text").GetComponent<Text>().text = "Play";
				pause_toggle = true;

				if (null != coroutine) {
					StopCoroutine(coroutine);
				}
				coroutine = null;
			}
			else {
				pause.transform.Find("Text").GetComponent<Text>().text = "Pause";
				pause_toggle = false;
				coroutine = StartCoroutine(SendHeartBeat());
			}
		});
		close.onClick.AddListener(() => {
			session.Close();
		});

        session.onConnect += () => {
            msg_seq = 1;
			MsgCliSvr_Login_Req req = new MsgCliSvr_Login_Req();
			req.user_id = SystemInfo.deviceUniqueIdentifier;

			Log("MsgCliSvr_Login_Req(user_id:" + req.user_id + ")");
            session.SendMsg(req);
		};
		session.onReconnect += () => {
			/*
			MsgCliSvr_Reconnect_Req req = new MsgCliSvr_Reconnect_Req();
			req.user_id = user_data.user_id;
			req.access_token = user_data.access_token;

			Log("MsgCliSvr_Reconnect_Req(user_id:" + req.user_id + ", access_token:" + req.access_token +")");
            session.SendMsg(req);
            */
		};
		session.onClose += () => {
			Log("session close");
        };
		session.onError += (System.Exception e) => {
			Log(e.ToString());
			if(null != coroutine)
			{
				StopCoroutine(coroutine);
			}
			coroutine = null;
		};
		session.RegisterHandler (MsgSvrCli_Login_Ans.MSG_ID, (Gamnet.Buffer buffer) => {
			MsgSvrCli_Login_Ans ans = new MsgSvrCli_Login_Ans();
			if(false == ans.Load(buffer)) {
				Log("MessageFormatError(MsgSvrCli_Login_Ans)");
				return;
			}
				
			Log("MsgSvrCli_Login_Ans(user_seq:" + ans.user_data.user_seq + ", error_code:" + ans.error_code.ToString() +")");

			if(ErrorCode.Success != ans.error_code)	{
				session.Close();
				return;
			}

			user_data = ans.user_data;

			if(null != coroutine) {
				StopCoroutine(coroutine);
			}
			coroutine = StartCoroutine(SendHeartBeat());
		});
		session.RegisterHandler (MsgSvrCli_Reconnect_Ans.MSG_ID, (Gamnet.Buffer buffer) => {
			MsgSvrCli_Reconnect_Ans ans = new MsgSvrCli_Reconnect_Ans();
			if(false == ans.Load(buffer)) {
				Log("MessageFormatError(MsgSvrCli_Reconnect_Ans)");
				return ;
			}
			Log("MsgSvrCli_Reconnect_Ans(msg_seq:" + ans.msg_seq + ")");
			if(null != coroutine) {
				StopCoroutine(coroutine);
			}
			coroutine = StartCoroutine(SendHeartBeat());
		});
		session.RegisterHandler (MsgSvrCli_HeartBeat_Ntf.MSG_ID, (Gamnet.Buffer buffer) => {
			MsgSvrCli_HeartBeat_Ntf ntf = new MsgSvrCli_HeartBeat_Ntf();
			if(false == ntf.Load(buffer)) {
				Log("MessageFormatError(MsgSvrCli_HeartBeat_Ntf)");
				return;
			}

			Log("MsgSvrCli_HeartBeat_Ntf(msg_seq:" + ntf.msg_seq.ToString() + ")");
			session.RemoveSentPacket(ntf.msg_seq);
			// remove every queued message under msg_seq
		});
		session.RegisterHandler (MsgSvrCli_Kickout_Ntf.MSG_ID, (Gamnet.Buffer buffer) => {
			MsgSvrCli_Kickout_Ntf ntf = new MsgSvrCli_Kickout_Ntf();
			if(false == ntf.Load(buffer)) {
				Log("MessageFormatError(MsgSvrCli_Kickout_Ntf)");
				return;
			}

			Log("MsgSvrCli_Kickout_Ntf(error_code:" + ntf.error_code.ToString() + ")");
			session.Close();
            if (null != coroutine)
            {
                StopCoroutine(coroutine);
            }
			coroutine = null;
		});
	}

	IEnumerator SendHeartBeat() {
		while (true) {
			MsgCliSvr_HeartBeat_Ntf ntf = new MsgCliSvr_HeartBeat_Ntf();
            ntf.msg_seq = msg_seq++;
			Log("MsgCliSvr_HeartBeat_Ntf(msg_seq:" + ntf.msg_seq + ")");
			session.SendMsg (ntf, true);			
			yield return new WaitForSeconds (3.0f);
		}
	}

	// Update is called once per frame
	void Update () {
		session.Update ();
	}
		
    void Log(string text)
    {
        logText.text += lineNum.ToString() + ":" + text + System.Environment.NewLine;
        lineCount++;
        lineNum++;
        if (lineLimit < lineCount)
        {
            int index = logText.text.IndexOf(System.Environment.NewLine);
            logText.text = logText.text.Substring(index + System.Environment.NewLine.Length);
        }
        scrollRect.verticalNormalizedPosition = 0.0f;
    }
}
