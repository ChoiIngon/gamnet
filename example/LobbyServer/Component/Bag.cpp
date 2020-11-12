#include "Bag.h"
#include <Gamnet/Database/MySQL/MySQL.h>
#include <idl/MessageUser.h>
#include <idl/MessageItem.h>
#include "../UserSession.h"
#include "Item.h"
#include "Suit.h"

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
		std::list<std::shared_ptr<Item::Data>> equippedItems;
		for (auto& row : rows)
		{
			std::shared_ptr<Item::Data> item = Item::Create(row->getUInt32("item_index"), row->getInt("item_count"));
			item->session = session;
			item->seq = row->getInt64("item_seq");
			if(nullptr != item->expire)
			{
				item->expire->SetDate(row->getString("expire_date"));
			}
			if(nullptr != item->equip)
			{
				item->equip->part = (Message::EquipItemPartType)row->getInt32("equip_part");
				equippedItems.push_back(item);
			}
			last_item_seq = std::max(last_item_seq, item->seq);
			item_datas.insert(std::make_pair(item->seq, item));
			ntf.item_datas.push_back(*item);
		}
		if(0 < ntf.item_datas.size())
		{
			Gamnet::Network::Tcp::SendMsg(session, ntf, true);
		}
		
		if(0 < equippedItems.size())
		{
			std::shared_ptr<Component::Suit> suit = session->GetComponent<Component::Suit>();
			assert(nullptr != suit);
			for(std::shared_ptr<Item::Data>& item : equippedItems)
			{
				suit->EquipWithoutDB(item);
			}
		}
	}

	void Bag::Insert(const std::shared_ptr<Item::Data>& item)
	{
		std::shared_ptr<UserSession> session = this->session;
		const std::shared_ptr<Item::Meta>& meta = item->meta;
		std::list<std::shared_ptr<Item::Data>> addItems;
		
		if(true == item->count.Stackable())
		{
			int loop = item->count / meta->max_stack;
			for (int i = 0; i < loop; i++)
			{
				std::shared_ptr<Item::Data> portion = meta->CreateInstance();
				portion->seq = ++last_item_seq;
				portion->session = session;
				portion->count = meta->max_stack;
				if(nullptr != portion->expire)
				{
					portion->expire->TriggerExpire(Item::Meta::Expire::TriggerType::OnCreate);
				}

				session->queries->Insert("user_item", {
					{ "item_seq", portion->seq },
					{ "item_index", meta->index },
					{ "item_count", meta->max_stack },
					{ "expire_date", portion->GetExpireDate().ToString() },
					{ "user_seq", session->user_seq }
				});

				addItems.push_back(portion);
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

					session->queries->Update("user_item", 
						Gamnet::Format("item_count=", other->count), 
						{
							{ "user_seq", session->user_seq },
							{ "item_seq", other->seq }
						}
					);

					updatedItems.push_back(other);

					if (0 == item->count)
					{
						break;
					}
				}

				if(0 < updatedItems.size())
				{
					auto self = session;
					session->on_commit.push_back([=]()
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
				if (nullptr != item->expire)
				{
					item->expire->TriggerExpire(Item::Meta::Expire::TriggerType::OnCreate);
				}

				session->queries->Insert("user_item", {
					{ "item_seq", item->seq },
					{ "item_index", item->meta->index },
					{ "item_count", (int)item->count },
					{ "expire_date", item->GetExpireDate().ToString() },
					{ "user_seq", session->user_seq }
				});
				addItems.push_back(item);
			}
		}
		else 
		{
			item->seq = ++last_item_seq;
			item->session = session;
			if (nullptr != item->expire)
			{
				item->expire->TriggerExpire(Item::Meta::Expire::TriggerType::OnCreate);
			}

			session->queries->Insert("user_item", {
				{ "item_seq", item->seq },
				{ "item_index", meta->index },
				{ "item_count", 1 },
				{ "expire_date", item->GetExpireDate().ToString() },
				{ "user_seq", session->user_seq }
			});
			addItems.push_back(item);
		}

		session->on_commit.push_back([=]() {
			Message::Item::MsgSvrCli_AddItem_Ntf ntf;
			for(std::shared_ptr<Item::Data> data : addItems)
			{
				this->item_datas.insert(std::make_pair(data->seq, data));
				ntf.item_datas.push_back(*data);
			}
			Gamnet::Network::Tcp::SendMsg(session, ntf, true);
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

		if(true == item->count.Stackable())
		{
			if(count < item->count)
			{
				session->queries->Update("user_item", Gamnet::Format("item_count=", item->count), {
					{ "user_seq", session->user_seq },
					{ "item_seq", item->seq }
				});
				session->on_commit.push_back([=]() {
					item->count -= count;
					Message::Item::MsgSvrCli_UpdateItem_Ntf ntf;
					ntf.item_datas.push_back(*item);
					Gamnet::Network::Tcp::SendMsg(session, ntf);
				});
				return;
			}
		}
		
		session->queries->Update("user_item", "item_count=0,delete_date=NOW(),delete_yn='Y'", {
			{ "user_seq", session->user_seq },
			{ "item_seq", item->seq }
		});
		session->on_commit.push_back([=]() {
			this->item_datas.erase(itemSEQ);
			item->count -= count;
			Message::Item::MsgSvrCli_UpdateItem_Ntf ntf;
			ntf.item_datas.push_back(*item);
			Gamnet::Network::Tcp::SendMsg(session, ntf);
		});


	}
}

void Test_AddItem_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_AddItem_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	for (auto& item : ntf.item_datas)
	{
		session->items.insert(std::make_pair((int)item.item_seq, item));
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::Item::MsgSvrCli_AddItem_Ntf, Test_AddItem_Ntf
);

void Test_UpdateItem_Ntf(const std::shared_ptr<TestSession>& session, const std::shared_ptr<Gamnet::Network::Tcp::Packet>& packet)
{
	Message::Item::MsgSvrCli_UpdateItem_Ntf ntf;
	try {
		if (false == Gamnet::Network::Tcp::Packet::Load(ntf, packet))
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::MessageFormatError, "message load fail");
		}
	}
	catch (const Gamnet::Exception& e) {
		LOG(Gamnet::Log::Logger::LOG_LEVEL_ERR, e.what());
	}

	for (auto& item : ntf.item_datas)
	{
		if (0 == item.item_count)
		{
			session->items.erase(item.item_seq);
		}
		else
		{
			session->items[item.item_seq] = item;
		}
	}
}

GAMNET_BIND_TEST_RECV_HANDLER(
	TestSession, "",
	Message::Item::MsgSvrCli_UpdateItem_Ntf, Test_UpdateItem_Ntf
);