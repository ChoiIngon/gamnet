#include "Bag.h"
#include <Gamnet/Database/MySQL/MySQL.h>
#include <idl/MessageUser.h>
#include <idl/MessageItem.h>
#include "../UserSession.h"
#include "Item.h"

namespace Component {
	Bag::Bag(const std::shared_ptr<UserSession>& session)
		: session(session)
		, last_item_seq(0)
	{
	}

	void Bag::Load()
	{
		Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
			"SELECT item_seq, item_index, item_count, expire_date FROM ("
				"SELECT * FROM user_item WHERE user_seq=", session->user_seq, " AND delete_yn='N'"
			") a WHERE expire_date > NOW() AND item_seq > ", last_item_seq
		);

		Message::Item::MsgSvrCli_AddItem_Ntf ntf;
		for (auto& row : rows)
		{
			std::shared_ptr<Item::Data> item = Gamnet::Singleton<Item::Manager>::GetInstance().CreateInstance(session, row->getUInt32("item_index"), row->getInt("item_count"));
			item->seq = row->getInt64("item_seq");
			if(nullptr != item->expire)
			{
				item->expire->SetDate(row->getString("expire_date"));
			}
			last_item_seq = std::max(last_item_seq, item->seq);
			item_datas.insert(std::make_pair(item->seq, item));
			ntf.item_datas.push_back(*item);
		}
		if(0 < ntf.item_datas.size())
		{
			Gamnet::Network::Tcp::SendMsg(session, ntf, true);
		}
	}

	void Bag::Insert(const std::shared_ptr<Item::Data>& item)
	{
		const std::shared_ptr<Item::Meta>& meta = item->meta;
		if(1 < meta->max_stack)
		{
			int loop = item->count / meta->max_stack;
			for (int i = 0; i < loop; i++)
			{
				session->queries->Insert("user_item", {
					{ "item_index", meta->index },
					{ "item_count", meta->max_stack },
					{ "expire_date", item->GetExpireDate().ToString() },
					{ "user_seq", session->user_seq }
				});
				item->count -= item->meta->max_stack;
			}

			if (0 < item->count)
			{
				std::list<std::shared_ptr<Item::Data>> updatedItems;
				for (auto& itr : item_datas)
				{
					std::shared_ptr<Item::Data>& other = itr.second;
					if(false == Merge(other, item))
					{
						continue;
					}

					updatedItems.push_back(other);
					session->queries->Update("user_item", Gamnet::Format("item_count=", other->count), {
						{ "user_seq", session->user_seq },
						{ "item_seq", other->seq }
					});

					if (0 == item->count)
					{
						break;
					}
				}

				if(0 < updatedItems.size())
				{
					Message::Item::MsgSvrCli_UpdateItem_Ntf ntf;
					for(auto item : updatedItems)
					{
						ntf.item_datas.push_back(*item);
					}
					Gamnet::Network::Tcp::SendMsg(session, ntf);
				}
			}

			if (0 < item->count)
			{
				int count = std::min(item->count, meta->max_stack);
				session->queries->Insert("user_item", {
					{ "item_index", item->meta->index },
					{ "item_count", count },
					{ "expire_date", item->GetExpireDate().ToString() },
					{ "user_seq", session->user_seq }
				});
			}
		}
		else 
		{
			session->queries->Insert("user_item", {
				{ "item_index", meta->index },
				{ "item_count", 1 },
				{ "expire_date", item->GetExpireDate().ToString() },
				{ "user_seq", session->user_seq }
			});
		}
		session->on_commit["Bag"] = std::bind(&Bag::Load, this);
	}

	std::shared_ptr<Item::Data> Bag::Find(uint64_t itemSEQ)
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
		std::shared_ptr<Item::Data> item = Find(itemSEQ);
		if(nullptr == item)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		if(count > item->count)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}

		if(1 < item->meta->max_stack)
		{
			item->count -= count;
			if (0 < item->count)
			{
				session->queries->Update("user_item", Gamnet::Format("item_count=", item->count), {
					{ "item_seq", item->seq }
				});
				return;
			}
		}
		
		session->queries->Update("user_item", "delete_date=NOW(),delete_yn='Y'", {
			{ "item_seq", item->seq }
		});
		item_datas.erase(itemSEQ);
	}
}