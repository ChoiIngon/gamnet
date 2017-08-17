using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using message;

public class UnityClient : MonoBehaviour {
	private Gamnet.Session session = new Gamnet.Session();
	private Coroutine coroutine;
	uint seq = 0;
	UserData user_data;

	public Button connect;
	public Button close;
    public Button pause;
    bool pauseToggle = false;
	public InputField host;
    public ScrollRect scrollRect;    
	// Use this for initialization
	void Start () {
        connect.onClick.AddListener(() => {
            Log("connect to " + host.text);
            seq = 0;
            if ("" == host.text)
            {
                session.Connect("52.78.185.159", 20000, 60000);
            }
            else
            {
                session.Connect(host.text, 20000, 60000);
            }
        });

        session.onConnect += () => {
			Log("onConnect(host:" + host.text + ", port:20000)");
            Debug.Log("onConnect(host:" + host.text + ", port:20000)");

            MsgCliSvr_Login_Req req = new MsgCliSvr_Login_Req();
			req.user_id = "unity_client";
			req.access_token = "";

			Log("MsgCliSvr_Login_Req(user_id:" + req.user_id + ")");
            Debug.Log("MsgCliSvr_Login_Req(user_id:" + req.user_id + ")");
            session.SendMsg(req);
		};
		session.onReconnect += () => {
			Log("onReconnect(host:" + host.text + ", port:20000)");
            Debug.Log("onReconnect(host:" + host.text + ", port:20000)");

            MsgCliSvr_Login_Req req = new MsgCliSvr_Login_Req();
			req.user_id = user_data.user_id;
			req.access_token = user_data.access_token;

			Log("MsgCliSvr_Login_Req(user_id:" + req.user_id + ", access_token:" + req.access_token +")");
            Debug.Log("MsgCliSvr_Login_Req(user_id:" + req.user_id + ", access_token:" + req.access_token + ")");
            session.SendMsg(req);
		};

        close.onClick.AddListener(() => {
            Log("close button click");
            Debug.Log("close button click");
            session.Close();
        });
        session.onClose += () => {
			Log("session close");
            Debug.Log("session close");
        };
		session.onError += (System.Exception e) => {
			Log("session error(name:" + e.ToString() + ", message:" + e.Message + ", stack:" + e.StackTrace + ")");
		};
		session.RegisterHandler (MsgSvrCli_Login_Ans.MSG_ID, (Gamnet.Buffer buffer) => {
			MsgSvrCli_Login_Ans ans = new MsgSvrCli_Login_Ans();
			ans.Load(buffer);

			user_data = ans.user_data;
			Log("MsgSvrCli_Login_Ans(user_seq:" + user_data.user_seq + ", error_code:" + ans.error_code.ToString() +")");
			if(ERROR_CODE.ERROR_SUCCESS != ans.error_code)
			{
				user_data = null;
				return;
			}

			if(null != coroutine)
			{
				StopCoroutine(coroutine);
			}
			coroutine = StartCoroutine(SendHeartBeat());
		});
		session.RegisterHandler (MsgSvrCli_Kickout_Ntf.MSG_ID, (Gamnet.Buffer buffer) => {
			MsgSvrCli_Kickout_Ntf ntf = new MsgSvrCli_Kickout_Ntf();
			ntf.Load(buffer);

			Log("MsgSvrCli_Kickout_Ntf(error_code:" + ntf.error_code.ToString() + ")");
			session.Close();
            if (null != coroutine)
            {
                StopCoroutine(coroutine);
            }
			coroutine = null;
		});
		
        pause.onClick.AddListener(() =>
        {
            if (false == pauseToggle)
            {
                pause.transform.Find("Text").GetComponent<Text>().text = "Play";
                pauseToggle = true;

                if (null != coroutine)
                {
                    StopCoroutine(coroutine);
                }
                coroutine = null;
            }
            else
            {
                pause.transform.Find("Text").GetComponent<Text>().text = "Pause";
                pauseToggle = false;

                coroutine = StartCoroutine(SendHeartBeat());
            }
        });
	}
	
	// Update is called once per frame
	void Update () {
		session.Update ();
	}
		
	IEnumerator SendHeartBeat() {
		while (true) {
			yield return new WaitForSeconds (1.0f);
			MsgCliSvr_HeartBeat_Ntf ntf = new MsgCliSvr_HeartBeat_Ntf();
			ntf.msg_seq = ++seq;
            Log("MsgCliSvr_HeartBeat_Ntf(msg_seq:" + ntf.msg_seq + ")");
            session.SendMsg (ntf);			
		}
	}

    void Log(string text)
    {
        UILog.Instance.Write(text);
        scrollRect.verticalNormalizedPosition = 0.0f;
    }
}
