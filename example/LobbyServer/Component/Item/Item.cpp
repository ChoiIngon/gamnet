#include "Item.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>
#include "../../../idl/MessageCommon.h"
#include "../../UserSession.h"

ItemMeta::ItemMeta()
	: item_id(0)
	, item_type(ItemType::Invalid)
	, grade(0)
	, max_stack(0)
	, price(0)
	, expire_time(0)
	, expire_date(Gamnet::Time::DateTime::MaxValue)
{
	GAMNET_META_MEMBER(item_id);
	GAMNET_META_CUSTOM(item_type, ItemMeta::OnItemType);
	GAMNET_META_MEMBER(grade);
	GAMNET_META_MEMBER(max_stack);
	GAMNET_META_CUSTOM(price_type, ItemMeta::OnPriceType);
	GAMNET_META_MEMBER(price);
	GAMNET_META_MEMBER(expire_time);
	GAMNET_META_MEMBER(expire_date);
	GAMNET_META_MEMBER(packages);
}

void ItemMeta::OnItemType(ItemType& member, const std::string& value)
{
	member = Parse<ItemType>(value);
}

void ItemMeta::OnPriceType(CounterType& member, const std::string& value)
{
	member = Parse<CounterType>(value);
}

bool ItemMeta::OnLoad()
{
	if(0 >= grade || 10 < grade)
	{
		LOG(ERR, "item grade range error(grade:", grade, ")");
		return false;
	}
	return true;
}

void Manager_Item::Init()
{
	auto& metas = itemmeta_reader.Read("../MetaData/Item.csv");
	for(auto& meta : metas)
	{
		item_metas.insert(std::make_pair(meta->item_id, meta));
		random.push_back(meta);
	}
}

std::shared_ptr<ItemMeta> Manager_Item::FindMeta(int itemID)
{
	auto itr = item_metas.find(itemID);
	if(item_metas.end() == itr)
	{
		return nullptr;
	}
	return itr->second;
}

std::shared_ptr<ItemMeta> Manager_Item::RandMeta() const
{
	return random[Gamnet::Random::Range(0, random.size() - 1)];
}

std::shared_ptr<ItemData> Manager_Item::CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t itemID, int32_t count)
{
	std::shared_ptr<ItemMeta> meta = FindMeta(itemID);

	std::shared_ptr<ItemData> data = std::make_shared<ItemData>();
	data->meta = meta;

	if(Gamnet::Time::DateTime::MaxValue > meta->expire_date)
	{
		data->expire = std::make_shared<ItemData::Expire>();
		data->expire->expire_date = meta->expire_date;
	}

	if(0 != meta->expire_time)
	{
		data->expire = std::make_shared<ItemData::Expire>();
		data->expire->expire_date = Gamnet::Time::FromUnixtime(Gamnet::Time::Local::Now() + meta->expire_time);
	}

	if(0 < meta->packages.size())
	{
		data->package = std::make_shared<ItemData::Package>();
		for(uint32_t itemID : meta->packages)
		{
			std::shared_ptr<ItemMeta> package = FindMeta(itemID);
			if(nullptr == package)
			{
				throw GAMNET_EXCEPTION(ErrorCode::UndefineError);
			}
			data->package->item_metas.push_back(package);
		}
	}

	if(1 < meta->max_stack)
	{
		data->stack = std::make_shared<ItemData::Stack>();
		data->stack->cur_count = count;
		data->stack->max_count = meta->max_stack;
	}

	return data;
}

GAMNET_BIND_INIT_HANDLER(Manager_Item, Init);
namespace Item {
std::shared_ptr<ItemData> CreateInstance(const std::shared_ptr<UserSession>& session, uint32_t itemid, int32_t count)
{
	return Gamnet::Singleton<Manager_Item>::GetInstance().CreateInstance(session, itemid, count);
}
}