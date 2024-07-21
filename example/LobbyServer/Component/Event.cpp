#include "Event.h"
#include "../UserSession.h"
#include "Mail.h"
#include "Item.h"

namespace Component {
	EventMeta::EventMeta()
		: index(0)
		, mail_message("")
		, mail_expire_day(0)
		, item_id("")
		, item_count(0)
	{
		META_MEMBER(index);
		META_MEMBER(mail_message);
		META_MEMBER(mail_expire_day);
		META_MEMBER(item_id);
		META_MEMBER(item_count);
	};

	void EventMeta::OnLoad() 
	{
		if(nullptr == Gamnet::Singleton<Item::Manager>::GetInstance().FindMeta(item_id))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::InvalidItemIndex, "(event_index:", index, ", item_index:", item_id, ")");
		}
	}

	void Manager_Event::Init()
	{
		GAMNET_CALL_INIT_HANDLER(Item::Manager);
		reader.Read("../MetaData/Event.csv");
	}

	const MetaReader<EventMeta>::MetaDatas& Manager_Event::GetAllMetaData() const
	{
		return reader.GetAllMetaData();
	}

	void func()
	{
	}

	GAMNET_BIND_INIT_HANDLER(Manager_Event, Init);
	
	Event::Event(const std::shared_ptr<UserSession>& session)
		: session(session)
	{
	}

	void Event::Load()
	{
		const MetaReader<EventMeta>::MetaDatas& metas = Gamnet::Singleton<Manager_Event>::GetInstance().GetAllMetaData();
		if(metas.size() > events.size())
		{
			auto rows = Gamnet::Database::MySQL::Execute(session->shard_index,
				"SELECT event_index, update_date FROM user_event WHERE user_seq=", session->user_seq
			);

			for(auto& row : rows)
			{
				std::shared_ptr<Data> eventData = std::make_shared<Data>();
				eventData->index = row->getUInt32("event_index");
				eventData->update_date = row->getString("update_date");
				events[eventData->index] = eventData;
			}
		}

		for (auto& meta : metas)
		{
			auto itemMeta = Gamnet::Singleton<Item::Manager>::GetInstance().FindMeta(meta->item_id);
			auto itr = events.find(meta->index);
			if (events.end() == itr)
			{
				session->queries->Execute(
					"INSERT INTO user_event (user_seq, event_index, update_date) VALUES (", session->user_seq, ",", meta->index, ",NOW())"
				);
				std::shared_ptr<Data> eventData = std::make_shared<Data>();
				eventData->index = meta->index;
				eventData->update_date = Gamnet::Time::DateTime(Gamnet::Time::Local::Now());
				events[eventData->index] = eventData;

				std::shared_ptr<MailData> mailData = std::make_shared<MailData>();
				mailData->expire_date = Gamnet::Time::DateTime(Gamnet::Time::Local::Now() + meta->mail_expire_day * 86400);
				mailData->item_index = itemMeta->index;
				mailData->item_count = meta->item_count;
				mailData->mail_message = meta->mail_message;
				Mail::SendMail(session, mailData);
			}
			else
			{
				auto eventData = itr->second;
				if (1 <= Gamnet::Time::DateDiff(Gamnet::Time::DateTime(Gamnet::Time::Local::Now()), eventData->update_date))
				{
					session->queries->Execute("UPDATE user_event SET update_date=NOW() WHERE user_seq=", session->user_seq, " AND event_index=", eventData->index);
					std::shared_ptr<MailData> mailData = std::make_shared<MailData>();
					mailData->expire_date = Gamnet::Time::DateTime(Gamnet::Time::Local::Now() + meta->mail_expire_day * 86400);
					mailData->item_index = itemMeta->index;
					mailData->item_count = meta->item_count;
					mailData->mail_message = meta->mail_message;
					Mail::SendMail(session, mailData);
				}
			}
		}
	}
}