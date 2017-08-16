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

	// Use this for initialization
	void Start () {
		session.onConnect += () => {
			UILog.Instance.Write("success connect");
			MsgCliSvr_Login_Req req = new MsgCliSvr_Login_Req();
			req.user_id = "unity_client";
			req.access_token = "";
			session.SendMsg(req);
		};
		session.onReconnect += () => {
			UILog.Instance.Write("success re-connect");
			MsgCliSvr_Login_Req req = new MsgCliSvr_Login_Req();
			req.user_id = user_data.user_id;
			req.access_token = user_data.access_token;
			session.SendMsg(req);
		};
		session.onClose += () => {
			UILog.Instance.Write("session close");
		};
		session.onError += (System.Exception e) => {
			UILog.Instance.Write("session error(message:" + e.Message + ")");
		};
		session.RegisterHandler (MsgSvrCli_Login_Ans.MSG_ID, (Gamnet.Buffer buffer) => {
			MsgSvrCli_Login_Ans ans = new MsgSvrCli_Login_Ans();
			ans.Load(buffer);

			user_data = ans.user_data;
			UILog.Instance.Write("user_seq:" + user_data.user_seq);

			if(null != coroutine)
			{
				StopCoroutine(coroutine);
			}
			coroutine = StartCoroutine(SendMoveNtf());
		});

		connect.onClick.AddListener (() => {
			UILog.Instance.Write("connect to " + host.text.ToString());
            seq = 0;
			session.Connect (host.text.ToString(), 20000);
		});
		close.onClick.AddListener(() => {
			session.Close();
		});
	}
	
	// Update is called once per frame
	void Update () {
		session.Update ();
	}
		
	IEnumerator SendMoveNtf() {
		while (true) {
			yield return new WaitForSeconds (1.0f);
			MsgCliSvr_Move_Ntf ntf = new MsgCliSvr_Move_Ntf();
			ntf.seq = ++seq;
			ntf.x = Random.Range (0.0f, 1.0f);
			ntf.y = Random.Range (0.0f, 1.0f);
			session.SendMsg (ntf);
			UILog.Instance.Write ("MsgCliSvr_Move_Ntf(msg_seq:" + ntf.seq + ")");
		}
	}
}
