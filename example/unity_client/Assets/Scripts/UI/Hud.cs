using UnityEngine;
using UnityEngine.UI;

namespace UI
{
	public class Hud : MonoBehaviour
	{
		public Gold gold;
		public Button mail_button;
		public Button bag_button;
		public Button setting_button;

		private void Awake()
		{
			mail_button.onClick.AddListener(() =>
			{
				GameManager.Instance.ui.mailbox.gameObject.SetActive(true);
			});

			bag_button.onClick.AddListener(() =>
			{
				GameManager.Instance.ui.bag.gameObject.SetActive(true);
			});
		}

		private void OnEnable()
		{
			
		}
		private void OnDestroy()
		{
			
		}		
	}
}
