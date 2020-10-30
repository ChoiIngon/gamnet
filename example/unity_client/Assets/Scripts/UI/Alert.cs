using UnityEngine;
using UnityEngine.UI;

namespace UI
{
    public class Alert : MonoBehaviour
    {
        public Text title;
        public Text message;
        public Button ok;

		void Awake()
		{
            ok.onClick.AddListener(() =>
            {
                gameObject.SetActive(false);
            });
		}
		// Start is called before the first frame update
		private void OnDestroy()
		{
            ok.onClick.RemoveAllListeners();
		}
		public void Open(string title, string message)
        {
            this.title.text = title;
            this.message.text = message;
            gameObject.SetActive(true);
        }
    }
}