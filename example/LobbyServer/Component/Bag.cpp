#include "Bag.h"
#include <Gamnet/Database/MySQL/MySQL.h>
#include "../UserSession.h"
#include "Item/Item.h"

namespace Component {
	Bag::Bag(const std::shared_ptr<UserSession>& session)
		: session(session)
		, last_item_seq(0)
	{
	}

	void Bag::Load()
	{
		Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
			"SELECT item_seq, item_id, item_count, expire_date FROM user_item WHERE user_seq=", session->user_seq, " AND delete_yn='N' AND item_seq > ", last_item_seq
		);

		for (auto& row : rows)
		{
			std::shared_ptr<ItemData> item = Gamnet::Singleton<Manager_Item>::GetInstance().CreateInstance(session, row->getUInt32("item_id"), row->getInt32("item_count"));
			item->item_seq = row->getInt64("item_seq");
			if(nullptr != item->expire)
			{
				item->expire->expire_date = row->getString("expire_date");
			}
			last_item_seq = std::max(last_item_seq, item->item_seq);

			item_datas.insert(std::make_pair(item->item_seq, item));
		}
	}

	void Bag::Insert(std::shared_ptr<ItemData>& item)
	{
		if(nullptr != item->stack)
		{
			for(auto& itr : item_datas)
			{
				if(itr.second->meta->item_id == item->meta->item_id && itr.second->stack->max_count > itr.second->stack->cur_count)
				{
					int32_t count = std::min(itr.second->stack->max_count - itr.second->stack->cur_count, item->stack->cur_count); 
					itr.second->stack->cur_count += count;
					item->stack->cur_count -= count;

					session->queries->Update("user_item", Gamnet::Format("item_count=", itr.second->stack->cur_count), {
						{ "user_seq", session->user_seq },
						{ "item_seq", itr.second->item_seq }
					});

					if(0 == item->stack->cur_count)
					{
						break;
					}
				}
			}
		}

		session->queries->Insert("user_item", {
			{ "item_id", item->meta->item_id },
			{ "user_seq", session->user_seq }
		});
		session->on_commit["Bag"] = std::bind(&Bag::Load, this);
	}
}