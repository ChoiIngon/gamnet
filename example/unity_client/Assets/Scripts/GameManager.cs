using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEditor;
using System.Collections;

class GameManager : Util.MonoSingleton<GameManager>
{
	private Network.LobbySession lobby_session = new Network.LobbySession();

	public bool run_in_background = true;
	
	public Component.Lobby lobby = null;
	public Component.Dungeon dungeon = null;

	public Component.Counter counter = null;
	public Component.MailBox mailbox = null;
	public Component.Bag bag = null;
    public Component.Suit suit = null;

	public UI.UIMain ui = null;
	public Network.LobbySession LobbySession
	{
		get { return lobby_session; }
	}

	private void Awake()
	{
		DontDestroyOnLoad(transform.gameObject);
		if (true == Application.isEditor)
		{
			Application.runInBackground = run_in_background;
		}

		counter	= new Component.Counter();
		mailbox	= new Component.MailBox();
		bag		= new Component.Bag();
		suit	= new Component.Suit();
	}

	private IEnumerator Start()
	{
		yield return AssetBundleManager.Instance.LoadAssetBundle("MetaData");
		yield return AssetBundleManager.Instance.LoadAssetBundle("Sprites");

		Item.Manager.Instance.Init();
		lobby.gameObject.SetActive(true);
		ui.login.gameObject.SetActive(true);

		AssetBundleManager.Instance.UnloadAssetBundle("MetaData");
	}

	private void OnDestroy()
	{
		AssetBundleManager.Instance.UnloadAssetBundle("Sprites");
	}
	private void Update()
	{
		if (null != lobby_session)
		{
			lobby_session.Update();
		}
	}
}

