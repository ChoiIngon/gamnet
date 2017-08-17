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
	public InputField host;
    public ScrollRect scrollRect;    
	// Use this for initialization
	void Start () {
		session.onConnect += () => {
			Log("success connect(host:" + host.text + ", port:20000)");
			MsgCliSvr_Login_Req req = new MsgCliSvr_Login_Req();
			req.user_id = "unity_client";
			req.access_token = "";
			Log("MsgCliSvr_Login_Req(user_id:" + req.user_id + ")");
			session.SendMsg(req);
		};
		session.onReconnect += () => {
			Log("success re-connect(host:" + host.text + ", port:20000)");
			MsgCliSvr_Login_Req req = new MsgCliSvr_Login_Req();
			req.user_id = user_data.user_id;
			req.access_token = user_data.access_token;
			Log("MsgCliSvr_Login_Req(user_id:" + req.user_id + ", access_token:" + req.access_token +")");
			session.SendMsg(req);
		};
		session.onClose += () => {
			Log("session close");
		};
		session.onError += (System.Exception e) => {
			Log("session error(message:" + e.Message + ", stack:" + e.StackTrace + ")");
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
			StopCoroutine(coroutine);
			coroutine = null;
		});
		connect.onClick.AddListener (() => {
			Log("connect to " + host.text.ToString());
            seq = 0;
			session.Connect (host.text.ToString(), 20000, 60000);
		});
		close.onClick.AddListener(() => {
			session.Close();
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
			session.SendMsg (ntf);
			Log("MsgCliSvr_HeartBeat_Ntf(msg_seq:" + ntf.msg_seq + ")");
		}
	}

    void Log(string text)
    {
        UILog.Instance.Write(text);
        scrollRect.verticalNormalizedPosition = 0.0f;
    }
}
