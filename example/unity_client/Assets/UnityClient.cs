using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using message;

public class UnityClient : MonoBehaviour {
	private Gamnet.Session session = new Gamnet.Session();
	int seq = 0;
	UserData user_data;

	// Use this for initialization
	void Start () {
		session.onConnect += () => {
			Debug.Log("success connect");
			MsgCliSvr_Login_Req req = new MsgCliSvr_Login_Req();
			req.user_id = "unity_client";
			req.access_token = "";
			session.SendMsg(req);
		};
		session.onReconnect += () => {
			Debug.Log("success re-connect");
			MsgCliSvr_Login_Req req = new MsgCliSvr_Login_Req();
			req.user_id = "unity_client";
			req.access_token = "";
			session.SendMsg(req);
		};
		session.onClose += () => {
			Debug.Log("session close");
		};
		session.onError += (System.Exception e) => {
			Debug.Log("session error(message:" + e.Message + ")");
		};
		session.RegisterHandler (MsgSvrCli_Login_Ans.MSG_ID, (Gamnet.Buffer buffer) => {
			MsgSvrCli_Login_Ans ans = new MsgSvrCli_Login_Ans();
			ans.Load(buffer);

			user_data = ans.user_data;
			Debug.Log("user_seq:" + user_data.user_seq);

			StartCoroutine(SendMoveNtf());
		});
		session.Connect ("127.0.0.1", 20000);
	}
	
	// Update is called once per frame
	void Update () {
		session.Update ();
	}
		
	IEnumerator SendMoveNtf() {
		while (true) {
			yield return new WaitForSeconds (3.0f);
			MsgCliSvr_Move_Ntf ntf = new MsgCliSvr_Move_Ntf();
			ntf.seq = ++seq;
			ntf.x = Random.Range (0.0f, 1.0f);
			ntf.y = Random.Range (0.0f, 1.0f);
			session.SendMsg (ntf);
		}
	}
}
