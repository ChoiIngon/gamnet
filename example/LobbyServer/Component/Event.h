#ifndef _COMPONENT_EVENT_H
#define _COMPONENT_EVENT_H

#include <memory>
#include <Gamnet/Library/Time/Time.h>
#include <Gamnet/Library/MetaData.h>

class UserSession;

namespace Component {
	
	struct EventMeta : public Gamnet::MetaData
	{
		EventMeta();

		virtual bool OnLoad() override;
		uint32_t event_id;
		std::string mail_message;
		int mail_expire_day;
		uint32_t item_index;
		int item_count;
	};

	class Manager_Event 
	{
	public :
		void Init();
		const Gamnet::MetaReader<EventMeta>::MetaDatas& GetAllMetaData() const;
	private :
		Gamnet::MetaReader<EventMeta> reader;
	};

	class EventData
	{
	public :
		uint32_t event_id;
		Gamnet::Time::DateTime update_date;
	};

	class Event
	{
	public :
		Event(const std::shared_ptr<UserSession>& session);
		void Load();
	private :
		std::shared_ptr<UserSession> session;
		std::map<uint32_t, std::shared_ptr<EventData>> events;
	};

}
#endif
