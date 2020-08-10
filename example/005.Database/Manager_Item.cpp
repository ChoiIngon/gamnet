#include "Manager_Item.h"
#include <Gamnet/Library/Singleton.h>
#include <Gamnet/Library/Random.h>

ItemMeta::ItemMeta()
	: GAMNET_META_MEMBER(item_id)
	, GAMNET_META_MEMBER(name)
	, GAMNET_META_CUSTOM(type, ItemMeta::OnType, ItemMeta::Type::Invalid)
	, GAMNET_META_MEMBER(grade)
	, GAMNET_META_MEMBER(max_stack)
	, GAMNET_META_MEMBER(price)
{
}

void ItemMeta::OnType(Type& member, const std::string& value)
{
	if("ItemType_1" == value)
	{
		member = Type::ItemType_1;
		return;
	}
	else if("ItemType_2" == value)
	{
		member = Type::ItemType_2;
		return;
	}
	member = Type::Invalid;
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
	auto& metas = itemmeta_reader.Read("Meta/Item.csv");
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

GAMNET_BIND_INIT_HANDLER(Manager_Item, Init);