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

        public Session session = new Session();

        // Start is called before the first frame update

        void Awake()
        {
            session.Connect(host, port);
        }

        void Start()
        {

        }

        // Update is called once per frame
        void Update()
        {

        }
    }
}