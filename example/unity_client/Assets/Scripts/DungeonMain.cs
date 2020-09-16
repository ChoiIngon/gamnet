using UnityEngine;
using UnityEngine.SceneManagement;

public class DungeonMain : MonoBehaviour
{
	public Component.Dungeon dungeon = null;
    // Start is called before the first frame update
    void Start()
    {
		GameManager.Instance.scenes.dungeon_main = this;
		Handler.Dungeon.Handler_CreateDungeon.SendMsg();        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

	private void OnDestroy()
	{
		dungeon = null;
	}
}
