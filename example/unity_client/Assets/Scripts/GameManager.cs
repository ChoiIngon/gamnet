using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;

class GameManager : Util.MonoSingleton<GameManager>
{
	public class Scenes
	{
		public LobbyMain lobby_main = null;
		public DungeonMain dungeon_main = null;
	}

	public Gamnet.Session session = new Gamnet.Session();
	public bool run_in_background = true;
	public Scenes scenes = new Scenes();
	
	private void Awake()
	{
		DontDestroyOnLoad(transform.gameObject);
		if (true == Application.isEditor)
		{
			Application.runInBackground = run_in_background;
		}

		session.onError += (Gamnet.Exception e) =>
		{
			if (Gamnet.ErrorCode.ReconnectFailError == e.ErrorCode)
			{
				session.Close();
				SceneManager.LoadScene("SceneLobby");
			}
		};
	}

	private void Start()
	{
		if (true == Application.isEditor)
		{
			Application.runInBackground = run_in_background;
		}
	}

	private void OnApplicationPause(bool pause)
	{
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
		session.Close();
	}
	
	void Update()
	{
		session.Update();
	}
}

