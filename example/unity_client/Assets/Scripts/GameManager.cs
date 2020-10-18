using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;

class GameManager : Util.MonoSingleton<GameManager>
{
	public class Scenes
	{
		public Scene.Lobby.Main lobby = null;
		public Scene.Dungeon.Main dungeon = null;
	}

	public Gamnet.Session session = new Gamnet.Session();
	public bool run_in_background = true;
	public Scenes scenes = new Scenes();
	public Component.Counter counter = new Component.Counter();
	public Component.Bag bag = new Component.Bag();
	public Component.Mail mail = new Component.Mail();
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
				SceneManager.LoadScene(Scene.Lobby.Main.GetName());
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

