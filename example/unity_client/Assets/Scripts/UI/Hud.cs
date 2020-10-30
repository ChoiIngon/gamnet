using UnityEngine;
using UnityEngine.UI;

namespace UI
{
	public class Hud : MonoBehaviour
	{
		public Gold gold;
		public Button mail_button;
		public Button quest_button;
		public Button setting_button;

		private void Awake()
		{
			mail_button.onClick.AddListener(() =>
			{
				GameManager.Instance.ui.mailbox.gameObject.SetActive(true);
			});
		}
		private void OnDestroy()
		{
			
		}
	}
}
