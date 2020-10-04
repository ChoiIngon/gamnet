#include "Item.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>
#include "../UserSession.h"
#include "Bag.h"

namespace Component {

	struct PriceMeta : public Gamnet::MetaData
	{
		PriceMeta();

		void OnPriceType(Message::CounterType& member, const std::string& value)
		{
			member = Message::Parse<Message::CounterType>(value);
		}

		Message::CounterType type;
		int value;
	};

	PriceMeta::PriceMeta()
		: type(Message::CounterType::Invalid)
		, value(0)
	{
		GAMNET_META_CUSTOM(type, PriceMeta::OnPriceType);
		GAMNET_META_MEMBER(value);
	}

	struct ExpireMeta : public Gamnet::MetaData
	{
		enum Type
		{
			None = 0,
			Bag = 1,
			Equip = 2
		};

		ExpireMeta();

		void OnExpireType(Type& member, const std::string& value)
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
		Type type;
		int64_t time;
		Gamnet::Time::DateTime date;
	};

	ExpireMeta::ExpireMeta()
		: type(ExpireMeta::None)
		, time(0)
		, date(0)
	{
		GAMNET_META_CUSTOM(type, ExpireMeta::OnExpireType);
		GAMNET_META_MEMBER(time);
		GAMNET_META_MEMBER(date);
	}

	struct PackageMeta : public Gamnet::MetaData
	{
		PackageMeta();
		std::string id;
		int		 count;
	};

	PackageMeta::PackageMeta()
		: count(0)
	{
		GAMNET_META_MEMBER(id);
		GAMNET_META_MEMBER(count);
	}

	struct ItemRow : public Gamnet::MetaData
	{
		ItemRow();
		void OnItemType(Message::ItemType& member, const std::string& value);
		void OnPriceType(Message::CounterType& member, const std::string& value);
		void OnExpireType(ItemMeta::ExpireMeta::Type& member, const std::string& value);
		virtual bool OnLoad() override;

		std::string			id;
		uint32_t			index;
		Message::ItemType	type;
		int					grade;
		int					max_stack;

		PriceMeta price;
		ExpireMeta expire;
		std::vector<PackageMeta> packages;
	};

	ItemRow::ItemRow()
		: id("")
		, index(0)
		, type(Message::ItemType::Invalid)
		, grade(0)
		, max_stack(0)
	{
		GAMNET_META_MEMBER(id);
		GAMNET_META_MEMBER(index);
		GAMNET_META_CUSTOM(type, ItemRow::OnItemType);
		GAMNET_META_MEMBER(grade);
		GAMNET_META_MEMBER(max_stack);
		GAMNET_META_MEMBER(price);
		GAMNET_META_MEMBER(expire);
		GAMNET_META_MEMBER(packages);
	}

	void ItemRow::OnItemType(Message::ItemType& member, const std::string& value)
	{
		member = Message::Parse<Message::ItemType>(value);
	}

	void ItemRow::OnPriceType(Message::CounterType& member, const std::string& value)
	{
		member = Message::Parse<Message::CounterType>(value);
	}

	void ItemRow::OnExpireType(ItemMeta::ExpireMeta::Type& member, const std::string& value)
	{
		if("Bag" == value)
		{
			member = ItemMeta::ExpireMeta::Type::Bag;
			return;
		}
		else if ("Equip" == value)
		{
			member = ItemMeta::ExpireMeta::Type::Equip;
			return;
		}
		else if("None" == value)
		{
			member = ItemMeta::ExpireMeta::Type::None;
			return;
		}
		throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
	}	
	
	bool ItemRow::OnLoad()	 
	{
		return true;
	}
	
	ItemMeta::ItemMeta()
		: item_id(0)
		, item_type(Message::ItemType::Invalid)
		, grade(0)
		, max_stack(0)
		, price(nullptr)
		, expire(nullptr)
	{
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
			/*
			std::shared_ptr<ItemData> item = Item::CreateInstance(session, package->id, package->count);
			bag->Insert(item);
			*/
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
		if (other->meta->item_id != self->meta->item_id)
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
	Gamnet::MetaReader<ItemRow> reader;
	auto& rows = reader.Read("../MetaData/Item.csv");
	for(auto& row : rows)
	{
	/*
		std::shared_ptr<ItemMeta> meta = std::make_shared<ItemMeta>();
		meta->item_id = row->id;
		meta->item_type = row->type;
		meta->grade = row->grade;
		meta->max_stack = row->max_stack;
		if(ItemMeta::ExpireMeta::Type::None != row->expire_type)
		{
			meta->expire_meta = std::make_shared<ItemMeta::ExpireMeta>();
			meta->expire_meta->expire_type = row->expire_type;
			meta->expire_meta->expire_time = row->expire_time;
			meta->expire_meta->expire_date = row->expire_date;
			if(0 == meta->expire_meta->expire_time && Gamnet::Time::DateTime::MaxValue == meta->expire_meta->expire_date)
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}
		}

		if(Message::ItemType::Package == meta->item_type)
		{
			if(0 == row->package_item_ids.size())
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}

			if(row->package_item_ids.size() != row->package_item_counts.size())
			{
				throw GAMNET_EXCEPTION(Message::ErrorCode::UndefineError);
			}

			for(int i=0; i<row->package_item_ids.size(); i++)
			{
				std::shared_ptr<ItemMeta::PackageMeta> package = std::make_shared<ItemMeta::PackageMeta>();
				package->item_id = row->package_item_ids[i];
				package->item_count = row->package_item_counts[i];
				meta->package_metas.push_back(package);
			}
		}
		
		if(Message::CounterType::Invalid != row->type)
		{
			meta->price = std::make_shared<ItemMeta::PriceMeta>();
			meta->price->type = row->type;
			meta->price->value = row->value;
		}
		item_metas.insert(std::make_pair(meta->item_id, meta));
	*/
	}
}

std::shared_ptr<ItemMeta> Manager_Item::FindMeta(uint32_t itemIndex)
{
	auto itr = item_metas.find(itemIndex);
	if(item_metas.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}

std::shared_ptr<ItemData> Manager_Item::CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t itemIndex, int32_t count)
{
	std::shared_ptr<ItemMeta> meta = FindMeta(itemIndex);
	std::shared_ptr<ItemData> data = meta->CreateInstance(session);
	data->item_count = count;
	return data;
}

std::shared_ptr<ItemData> Manager_Item::CreateInstance(const std::shared_ptr<UserSession>& session, const std::string& itemID, int32_t count)
{
	return nullptr;
}

GAMNET_BIND_INIT_HANDLER(Manager_Item, Init);
}
namespace Item {
std::shared_ptr<Component::ItemData> CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t itemIndex, int32_t count)
{
	return Gamnet::Singleton<Component::Manager_Item>::GetInstance().CreateInstance(session, itemIndex, count);
}
}