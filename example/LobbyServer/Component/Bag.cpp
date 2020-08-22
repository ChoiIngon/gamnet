#include "Bag.h"
#include <Gamnet/Database/MySQL/MySQL.h>
#include "../UserSession.h"
#include "Item.h"
#include "../../idl/MessageUser.h"
#include "../../idl/MessageItem.h"

namespace Component {
	Bag::Bag(const std::shared_ptr<UserSession>& session)
		: session(session)
		, last_item_seq(0)
	{
	}

	void Bag::Load()
	{
		Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
			"SELECT item_seq, item_id, item_count, expire_date FROM ("
				"SELECT * FROM user_item WHERE user_seq=", session->user_seq, " AND delete_yn='N'"
			") a WHERE expire_date > NOW() AND item_seq > ", last_item_seq
		);

		Message::Item::MsgSvrCli_Item_Ntf ntf;
		for (auto& row : rows)
		{
			std::shared_ptr<ItemData> item = Gamnet::Singleton<Manager_Item>::GetInstance().CreateInstance(session, row->getUInt32("item_id"), row->getInt32("item_count"));
			item->item_seq = row->getInt64("item_seq");
			item->expire_date = row->getString("expire_date");
			last_item_seq = std::max(last_item_seq, item->item_seq);
			item_datas.insert(std::make_pair(item->item_seq, item));
			ntf.item_datas.push_back(*item);
		}
		if(0 < ntf.item_datas.size())
		{
			Gamnet::Network::Tcp::SendMsg(session, ntf, true);
		}
	}

	void Bag::Insert(const std::shared_ptr<ItemData>& item)
	{
		if(nullptr != item->expire)
		{
			if(ItemMeta::ExpireMeta::Type::Bag == item->meta->expire_meta->expire_type)
			{
				item->expire->StartExpire();
			}
		}

		if(nullptr == item->stack)
		{
			session->queries->Insert("user_item", {
				{ "item_id", item->meta->item_id },
				{ "item_count", 1 },
				{ "expire_date", item->expire_date.ToString() },
				{ "user_seq", session->user_seq }
			});
		}
		else {
			int loop = item->item_count / item->meta->max_stack;
			for(int i=0; i<loop; i++)
			{
				session->queries->Insert("user_item", {
					{ "item_id", item->meta->item_id },
					{ "item_count", item->meta->max_stack },
					{ "expire_date", item->expire_date.ToString() },
					{ "user_seq", session->user_seq }
				});
				item->item_count -= item->meta->max_stack;
			}

			for(auto& itr : item_datas)
			{
				auto other = itr.second;
				if(nullptr == other->stack)
				{
					continue;
				}

				other->stack->Merge(item);
				if(0 == item->item_count)
				{
					break;
				}
			}

			if(0 < item->item_count)
			{
				int count = std::min(item->item_count, item->meta->max_stack);
				session->queries->Insert("user_item", {
					{ "item_id", item->meta->item_id },
					{ "item_count", item->item_count },
					{ "expire_date", item->expire_date.ToString() },
					{ "user_seq", session->user_seq }
				});
			}
		}
		session->on_commit["Bag"] = std::bind(&Bag::Load, this);
	}

	std::shared_ptr<ItemData> Bag::Find(uint64_t itemSEQ)
	{
		auto itr = item_datas.find(itemSEQ);
		if(item_datas.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	void Bag::Remove(uint64_t itemSEQ, int count)
	{
		std::shared_ptr<ItemData> item = Find(itemSEQ);
		if(nullptr == item)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		if(count > item->item_count)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}

		item->item_count -= count;
		if (0 < item->item_count)
		{
			session->queries->Update("user_item", Gamnet::Format("item_count=", item->item_count), {
				{ "item_seq", item->item_seq }
			});
		}
		else
		{
			session->queries->Update("user_item", "delete_date=NOW(),delete_yn='Y'", {
				{ "item_seq", item->item_seq }
			});
			item_datas.erase(itemSEQ);
		}
	}
}