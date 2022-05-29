using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace BasicStandAloneServer
{
    public class GameManager : MonoBehaviour
    {
        public Button connect;
        public Button reconnect;
        public Button pause;
        public Button close;

        public string host;
        public int port;

        public class Session : Gamnet.Session
        {
            public Session()
            {
            }

            private void OnApplicationPause(bool pause)
            {
                if (true == pause)
                {
                    Pause();
                }
                else
                {
                    Resume();
                }
            }

            private void OnApplicationQuit()
            {
                base.Close();
            }
        }

        public Session session;

        // Start is called before the first frame update

        void Awake()
        {
            session = new Session();
            session.onConnect += () =>
            {
                Debug.Log("success to make connection");
            };

            session.onClose += () =>
            {
            };

            session.onError += (Gamnet.Exception e) =>
            {
            };

            session.RegisterHandler<MsgSvrCli_Welcome_Ntf>((MsgSvrCli_Welcome_Ntf ntf) =>
            {
                Debug.Log($"welcom message from server '{ntf.greeting}'");

                MsgCliSvr_HelloWorld_Req req = new MsgCliSvr_HelloWorld_Req();
                req.greeting = "Hello World";
                session.SendMsg(req);
            });

            session.RegisterHandler<MsgSvrCli_HelloWorld_Ans>((MsgSvrCli_HelloWorld_Ans ans) =>
            {
                Debug.Log($"hello world ans from server");
            });

            connect.onClick.AddListener(() =>
            {
                session.Connect(host, port);
            });

            reconnect.onClick.AddListener(() =>
            {
                session.Resume();
            });

            pause.onClick.AddListener(() =>
            {
                session.Pause();
            });

            close.onClick.AddListener(() =>
            {
                session.Close();
            });
        }

        void Start()
        {

        }

        void Update()
        {
            if (null != session)
            {
                session.Update();
            }
        }

        private void OnDestroy()
        {
            session.onConnect = null;
            session.onClose = null;
            session.onError = null;

            session.UnregisterHandler<MsgSvrCli_Welcome_Ntf>();
            session.UnregisterHandler<MsgSvrCli_HelloWorld_Ans>();
            session = null;
        }
    }
}