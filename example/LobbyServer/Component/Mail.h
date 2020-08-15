#ifndef _COMPONENT_MAIL_H
#define _COMPONENT_MAIL_H

#include <memory>

class UserSession;

namespace Component {

	class Mail
	{
	public:
		class Data
		{
			uint64_t mail_seq;
		};

		Mail();
		virtual int Increase(int amount);
		virtual int Count();

	public:
		uint64_t last_mail_seq;
	private:
		std::shared_ptr<UserSession> session;
	};

}
#endif
