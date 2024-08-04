#ifndef _COMPONENT_EVENT_H
#define _COMPONENT_EVENT_H

#include <memory>
#include <Gamnet/Library/Time/Time.h>
#include "../Util/Table.h"

class UserSession;

namespace Component {
	
	struct EventMeta : public Table::MetaData<EventMeta>
	{
		EventMeta();

		virtual void OnLoad() override;

		uint32_t index;
		std::string mail_message;
		int mail_expire_day;
		std::string item_id;
		int item_count;
	};

	/*
	class Manager_Event 
	{
	public :
		void Init();
		const Table::MetaReader<EventMeta>::MetaDatas& GetAllMetaData() const;
	private :
		Table::MetaReader<EventMeta> reader;
	};
	*/
	class Event
	{
	public :
		class Data
		{
		public:
			uint32_t index;
			Gamnet::Time::DateTime update_date;
		};

	public :
		Event(const std::shared_ptr<UserSession>& session);

		void Load();
	private :
		std::shared_ptr<UserSession> session;
		std::map<uint32_t, std::shared_ptr<Data>> events;
	};

}
#endif
