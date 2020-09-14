using UnityEngine;
using UnityEngine.SceneManagement;

public class DungeonMain : MonoBehaviour
{
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
}
