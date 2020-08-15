#ifndef _COMPONENT_MAIL_H
#define _COMPONENT_MAIL_H

#include <memory>
#include <Gamnet/Library/Time/DateTime.h>

class UserSession;

namespace Component {

	struct MailData
	{
		uint64_t	mail_seq;
		std::string mail_message;
		uint32_t	item_id;
		int			item_count;
		Gamnet::Time::DateTime expire_date;
	};

	class Mail
	{
	public:
		Mail(const std::shared_ptr<UserSession>& session);
		void Load();
		void Open(uint64_t mailSEQ);
		static void SendMail(const std::shared_ptr<UserSession>& session, const std::shared_ptr<MailData>& mail);
	private:
		uint64_t last_mail_seq;
		time_t last_update_time;
		std::shared_ptr<UserSession> session;
		std::map<uint64_t, std::shared_ptr<MailData>> mail_datas;
	};

}
#endif
