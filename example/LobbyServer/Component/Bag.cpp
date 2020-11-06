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
		last_item_seq = 0;
		Gamnet::Database::MySQL::ResultSet rows = Gamnet::Database::MySQL::Execute(session->shard_index,
			"SELECT item_seq, item_index, item_count, equip_part, expire_date "
			"FROM user_item "
			"WHERE user_seq = ", session->user_seq, " AND NOW() < expire_date AND delete_yn='N'"
		);

		Message::Item::MsgSvrCli_AddItem_Ntf ntf;
		for (auto& row : rows)
		{
			std::shared_ptr<Item::Data> item = Item::Create(row->getUInt32("item_index"), row->getInt("item_count"));
			item->session = session;
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
		uint64_t prev_item_seq = last_item_seq;
		if(true == item->count.Stackable())
		{
			int loop = item->count / meta->max_stack;
			for (int i = 0; i < loop; i++)
			{
				std::shared_ptr<Item::Data> portion = meta->CreateInstance();
				portion->seq = ++last_item_seq;
				portion->session = session;
				portion->count = meta->max_stack;

				session->queries->Insert("user_item", {
					{ "item_seq", portion->seq },
					{ "item_index", meta->index },
					{ "item_count", meta->max_stack },
					{ "expire_date", portion->GetExpireDate().ToString() },
					{ "user_seq", session->user_seq }
				});
				item_datas.insert(std::make_pair(portion->seq, portion));
			}

			item->count -= item->meta->max_stack * loop;

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
					auto self = session;
					session->on_commit.push_back([self, updatedItems]()
					{
						Message::Item::MsgSvrCli_UpdateItem_Ntf ntf;
						for (auto item : updatedItems)
						{
							ntf.item_datas.push_back(*item);
						}
						Gamnet::Network::Tcp::SendMsg(self, ntf);
					});
				}
			}

			if (0 < item->count)
			{
				item->seq = ++last_item_seq;
				item->session = session;

				int count = std::min((int)item->count, meta->max_stack);
				session->queries->Insert("user_item", {
					{ "item_seq", item->seq },
					{ "item_index", item->meta->index },
					{ "item_count", count },
					{ "expire_date", item->GetExpireDate().ToString() },
					{ "user_seq", session->user_seq }
				});
				item_datas.insert(std::make_pair(item->seq, item));
			}
		}
		else 
		{
			item->seq = ++last_item_seq;
			item->session = session;

			session->queries->Insert("user_item", {
				{ "item_seq", item->seq },
				{ "item_index", meta->index },
				{ "item_count", 1 },
				{ "expire_date", item->GetExpireDate().ToString() },
				{ "user_seq", session->user_seq }
			});
			item_datas.insert(std::make_pair(item->seq, item));
		}
		auto self = session;
		uint64_t last_item_seq = this->last_item_seq;
		session->on_commit.push_back([self, prev_item_seq, last_item_seq]() {
			std::shared_ptr<Bag> bag = self->GetComponent<Component::Bag>();
			Message::Item::MsgSvrCli_AddItem_Ntf ntf;
			for(uint64_t i=prev_item_seq+1; i<=last_item_seq; i++)
			{
				std::shared_ptr<Item::Data> item = bag->Find(i);
				ntf.item_datas.push_back(*item);
			}
			Gamnet::Network::Tcp::SendMsg(self, ntf, true);
		});
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