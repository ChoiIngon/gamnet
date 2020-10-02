using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class DungeonMain : MonoBehaviour
{
	public Component.Dungeon dungeon = null;
	public Unit player;
	public Dictionary<UInt64, Unit> monsters = null;
	
    // Start is called before the first frame update
    void Start()
    {
		if (Gamnet.Session.ConnectionState.Connected != GameManager.Instance.session.state)
		{
			SceneManager.LoadScene("SceneLobby");
			return;
		}
		GameManager.Instance.scenes.dungeon_main = this;
		monsters = new Dictionary<ulong, Unit>();
		Handler.Dungeon.Handler_CreateDungeon.SendMsg();     
    }

    // Update is called once per frame
    void Update()
    {
        
    }

	private void OnDestroy()
	{
		dungeon = null;
		player = null;
		monsters = null;
	}
}
