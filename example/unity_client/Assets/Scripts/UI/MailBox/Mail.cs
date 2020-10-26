using System;
using UnityEngine;
using UnityEngine.UI;

namespace UI
{
    public class Mail : MonoBehaviour
    {
        public UInt64 mail_seq;
        public Text mail_message;
        public Text expire_date;
        public Text item_count;

        public void SetMailData(Message.MailData data)
        {
            mail_seq = data.mail_seq;
            mail_message.text = data.mail_message;
            DateTime dateTime = new DateTime(1970, 1, 1, 0, 0, 0);
            dateTime.AddSeconds(data.expire_date);
            expire_date.text = dateTime.ToString("yyyy-MM-dd HH:mm:ss");
        }
    }

}