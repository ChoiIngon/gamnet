#include "Item.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>
#include "../UserSession.h"
#include "Bag.h"

namespace Component {

	ItemMeta::PriceMeta::PriceMeta()
		: type(Message::CounterType::Invalid)
		, value(0)
	{
		GAMNET_META_CUSTOM(type, PriceMeta::OnPriceType);
		GAMNET_META_MEMBER(value);
	}

	void ItemMeta::PriceMeta::OnPriceType(Message::CounterType& member, const std::string& value)
	{
		member = Message::Parse<Message::CounterType>(value);
	}

	ItemMeta::ExpireMeta::ExpireMeta()
		: type(ExpireMeta::None)
		, time(0)
		, date(0)
	{
		GAMNET_META_CUSTOM(type, ExpireMeta::OnExpireType);
		GAMNET_META_MEMBER(time);
		GAMNET_META_MEMBER(date);
	}

	void ItemMeta::ExpireMeta::OnExpireType(Type& member, const std::string& value)
	{
		if ("Bag" == value)
		{
			member = Type::Bag;
			return;
		}
		else if ("Equip" == value)
		{
			member = Type::Equip;
			return;
		}
		else if ("None" == value)
		{
			member = Type::None;
			return;
		}
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}

	ItemMeta::PackageMeta::PackageMeta()
		: count(0)
	{
		GAMNET_META_MEMBER(id);
		GAMNET_META_MEMBER(count);
	}
	
	ItemMeta::ItemMeta()
		: id("")
		, index(0)
		, type(Message::ItemType::Invalid)
		, grade(0)
		, max_stack(0)
	{
		GAMNET_META_MEMBER(id);
		GAMNET_META_MEMBER(index);
		GAMNET_META_CUSTOM(type, ItemMeta::OnItemType);
		GAMNET_META_MEMBER(grade);
		GAMNET_META_MEMBER(max_stack);
		GAMNET_META_MEMBER(price);
		GAMNET_META_MEMBER(expire);
		GAMNET_META_MEMBER(packages);
	}

	void ItemMeta::OnItemType(Message::ItemType& member, const std::string& value)
	{
		member = Message::Parse<Message::ItemType>(value);
	}

	std::shared_ptr<ItemData> ItemMeta::CreateInstance(const std::shared_ptr<UserSession>& session)
	{
		std::shared_ptr<ItemData> item = std::make_shared<ItemData>(session, shared_from_this());
		if(nullptr != expire)
		{
			item->expire = std::make_shared<ItemData::Expire>(item);
		}

		if (0 < packages.size())
		{
			item->package = std::make_shared<ItemData::Package>(item);
		}

		if (1 < max_stack)
		{
			item->stack = std::make_shared<ItemData::Stack>(item);
		}

		return item;
	}

	ItemData::ItemData(const std::shared_ptr<UserSession>& session, const std::shared_ptr<ItemMeta>& meta)
		: item_seq(0)
		, item_count(0)
		, expire_date(Gamnet::Time::DateTime::MaxValue)
		, meta(meta)
		, expire(nullptr)
		, package(nullptr)
		, stack(nullptr)
		, session(session)
	{
	}

	ItemData::Expire::Expire(const std::shared_ptr<ItemData>& item)
		: item_data(item)
	{
	}

	void ItemData::Expire::StartExpire()
	{
		auto self = item_data.lock();
		if(Gamnet::Time::DateTime::MaxValue > self->expire_date)
		{
			return;
		}
		auto relative = Gamnet::Time::DateTime(Gamnet::Time::Local::Now() + self->meta->expire->time);
		self->expire_date = std::min(self->meta->expire->date, relative);
	}
	
	ItemData::Package::Package(const std::shared_ptr<ItemData>& item)
		: item_data(item)
	{
	}

	void ItemData::Package::Use()
	{
		auto self = item_data.lock();
		if(0 >= self->item_count)
		{
			throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
		}
		auto session = self->session.lock();
		auto bag = session->GetComponent<Component::Bag>();
		std::shared_ptr<ItemMeta> meta = item_data.lock()->meta;
		for(auto& package : meta->packages)
		{
			std::shared_ptr<ItemData> item = Item::CreateInstance(session, package->id, package->count);
			bag->Insert(item);
		}
		bag->Remove(self->item_seq, 1);
	}

	ItemData::Stack::Stack(const std::shared_ptr<ItemData>& item)
		: item_data(item)
	{
	}

	void ItemData::Stack::Merge(const std::shared_ptr<ItemData>& other)
	{
		auto self = item_data.lock();
		if (other->meta->id != self->meta->id)
		{
			return;
		}

		if (other->expire_date != self->expire_date)
		{
			return;
		}

		if (self->meta->max_stack <= self->item_count)
		{
			return;
		}

		int32_t count = std::min(self->meta->max_stack - self->item_count, other->item_count);
		self->item_count += count;
		other->item_count -= count;
		
		auto session = self->session.lock();
		session->queries->Update("user_item", Gamnet::Format("item_count=", self->item_count), {
			{ "item_seq", self->item_seq }
		});
	}
			
	void Manager_Item::Init()
	{
		Gamnet::MetaReader<ItemMeta> reader;
		auto& rows = reader.Read("../MetaData/Item.csv");
		for (auto& row : rows)
		{
			if (false == id_metas.insert(std::make_pair(row->id, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item id(", row->id, ")");
			}
			if (false == index_metas.insert(std::make_pair(row->index, row)).second)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError, "duplicate item index(", row->index, ")");
			}
		}
	}

	std::shared_ptr<ItemData> Manager_Item::CreateInstance(const std::shared_ptr<UserSession>& session, const std::string& id, int32_t count)
	{
		std::shared_ptr<ItemMeta> meta = FindMeta(id);
		std::shared_ptr<ItemData> data = meta->CreateInstance(session);
		data->item_count = count;
		return data;
	}

	std::shared_ptr<ItemData> Manager_Item::CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t index, int32_t count)
	{
		std::shared_ptr<ItemMeta> meta = FindMeta(index);
		std::shared_ptr<ItemData> data = meta->CreateInstance(session);
		data->item_count = count;
		return data;
	}

	std::shared_ptr<ItemMeta> Manager_Item::FindMeta(const std::string& id)
	{
		auto itr = id_metas.find(id);
		if (id_metas.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	std::shared_ptr<ItemMeta> Manager_Item::FindMeta(uint32_t index)
	{
		auto itr = index_metas.find(index);
		if(index_metas.end() == itr)
		{
			return nullptr;
		}
		return itr->second;
	}

	GAMNET_BIND_INIT_HANDLER(Manager_Item, Init);
}

namespace Item {
	std::shared_ptr<Component::ItemData> CreateInstance(const std::shared_ptr<UserSession>& session, const std::string& id, int32_t count)
	{
		return Gamnet::Singleton<Component::Manager_Item>::GetInstance().CreateInstance(session, id, count);
	}

	std::shared_ptr<Component::ItemData> CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t index, int32_t count)
	{
		return Gamnet::Singleton<Component::Manager_Item>::GetInstance().CreateInstance(session, index, count);
	}
}