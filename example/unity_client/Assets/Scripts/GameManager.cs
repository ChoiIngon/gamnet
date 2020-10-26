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
	public ResourceManager resource = new ResourceManager();

	public Component.Counter counter = null;
	public Component.Bag bag = null;
	public Component.MailBox mailbox = null;

	public UI.Main ui;
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
		
		session = new Gamnet.Session();
		resource = new ResourceManager();

		scenes = new Scenes();

		counter = new Component.Counter();
		bag = new Component.Bag();
		mailbox = new Component.MailBox();
		ui = gameObject.AddComponent<UI.Main>();
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

