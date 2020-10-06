#include "Event.h"
#include "../UserSession.h"
#include "Mail.h"
#include "Item.h"

namespace Component {
	EventMeta::EventMeta()
		: event_id(0)
		, mail_message("")
		, mail_expire_day(0)
		, item_index(0)
		, item_count(0)
	{
		GAMNET_META_MEMBER(event_id);
		GAMNET_META_MEMBER(mail_message);
		GAMNET_META_MEMBER(mail_expire_day);
		GAMNET_META_MEMBER(item_index);
		GAMNET_META_MEMBER(item_count);
	};

	bool EventMeta::OnLoad() 
	{
		if(nullptr == Gamnet::Singleton<Item::Manager>::GetInstance().FindMeta(item_index))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidItemIndex, "(event_id:", event_id, ", item_index:", item_index, ")");
		}
		return true;
	}

	void Manager_Event::Init()
	{
		reader.Read("../MetaData/Event.csv");
	}

	const Gamnet::MetaReader<EventMeta>::MetaDatas& Manager_Event::GetAllMetaData() const
	{
		return reader.GetAllMetaData();
	}

	GAMNET_BIND_INIT_HANDLER(Manager_Event, Init);

	Event::Event(const std::shared_ptr<UserSession>& session)
		: session(session)
	{
	}

	void Event::Load()
	{
		const Gamnet::MetaReader<EventMeta>::MetaDatas& metas = Gamnet::Singleton<Manager_Event>::GetInstance().GetAllMetaData();
		if(metas.size() > events.size())
		{
			auto rows = Gamnet::Database::MySQL::Execute(session->shard_index,
				"SELECT event_id, update_date FROM user_event WHERE user_seq=", session->user_seq
			);

			for(auto& row : rows)
			{
				std::shared_ptr<EventData> eventData = std::make_shared<EventData>();
				eventData->event_id = row->getUInt32("event_id");
				eventData->update_date = row->getString("update_date");
				events[eventData->event_id] = eventData;
			}
		}

		for (auto& meta : metas)
		{
			auto itr = events.find(meta->event_id);
			if (events.end() == itr)
			{
				session->queries->Execute(
					"INSERT INTO user_event (user_seq, event_id, update_date) VALUES (", session->user_seq, ",", meta->event_id, ",NOW())"
				);
				std::shared_ptr<EventData> eventData = std::make_shared<EventData>();
				eventData->event_id = meta->event_id;
				eventData->update_date = Gamnet::Time::DateTime(Gamnet::Time::Local::Now());
				events[eventData->event_id] = eventData;

				std::shared_ptr<MailData> mailData = std::make_shared<MailData>();
				mailData->expire_date = Gamnet::Time::DateTime(Gamnet::Time::Local::Now() + meta->mail_expire_day * 86400);
				mailData->item_index = meta->item_index;
				mailData->item_count = meta->item_count;
				mailData->mail_message = meta->mail_message;
				Mail::SendMail(session, mailData);
			}
			else
			{
				auto eventData = itr->second;
				if (1 <= Gamnet::Time::DateDiff(Gamnet::Time::DateTime(Gamnet::Time::Local::Now()), eventData->update_date))
				{
					session->queries->Execute("UPDATE user_event SET update_date=NOW() WHERE user_seq=", session->user_seq, " AND event_id=", eventData->event_id);
					std::shared_ptr<MailData> mailData = std::make_shared<MailData>();
					mailData->expire_date = Gamnet::Time::DateTime(Gamnet::Time::Local::Now() + meta->mail_expire_day * 86400);
					mailData->item_index = meta->item_index;
					mailData->item_count = meta->item_count;
					mailData->mail_message = meta->mail_message;
					Mail::SendMail(session, mailData);
				}
			}
		}
	}
}