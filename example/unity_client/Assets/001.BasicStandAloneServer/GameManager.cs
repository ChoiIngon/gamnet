using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace BasicStandAloneServer
{
    public class GameManager : MonoBehaviour
    {
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

        }

        void Start()
        {

        }

        private void OnEnable()
        {
            session.Connect(host, port);
        }

        private void OnDisable()
        {
            session.Close();
        }
        // Update is called once per frame
        void Update()
        {
            session.Update();
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